
#define __LIBVLC__ 1
#include "censor.hpp"
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_interface.h>
#include <vlc_aout.h>
//#include <vlc_fixups.h>
#include "CensorParser.h"
#include "CensorLexer.h"
#include <sys/poll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
  
static int  CensorActivate     ( vlc_object_t * p_this);
static void CensorClose        ( vlc_object_t * p_this);

vlc_module_begin();
    set_shortname(   "Censor");
    set_description( "auto-censor module");
    set_category(    CAT_ADVANCED );
    set_subcategory( SUBCAT_ADVANCED_MISC );
    set_callbacks(   CensorActivate, CensorClose );
    set_capability(  "interface",0);
    add_shortcut(    "censor" );
vlc_module_end();


/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
 

#define LESS_THAN -1
#define GREATER_THAN 1
#define EQUAL 0


/*****************************************************************************
 * Edit Collection struct and 
 *****************************************************************************/

static censor_edit_collection_t edit_collection; 
 

static dynamic_array_t * intersecting_edits( censor_edit_collection_t *edit_collection, struct timeval *t );

static struct timeval *absolute_time_to_next_edit( censor_edit_collection_t *edit_collection, struct timeval *movie_time);

//static void init_edit_collection(censor_edit_collection_t edit_collection);
// Implementation

typedef struct censor_edit_iter_t {
    dynamic_array_iter_t *_p_section_iter;
    dynamic_array_iter_t *_p_edit_iter;
} censor_edit_iter_t;

static censor_edit_iter_t * censor_edit_iter_new(censor_edit_collection_t *p_edit_collection) {
    censor_edit_iter_t *p_edit_iter;
    p_edit_iter = malloc(sizeof(*p_edit_iter));
    p_edit_iter->_p_section_iter = dynamic_array_iter_new(p_edit_collection->p_censor_sections);
    p_edit_iter->_p_edit_iter = NULL;
    return p_edit_iter;
}

//static void censor_edit_iter_free( censor_edit_iter_t *p_edit_iter) {
//    dynamic_array_iter_free(p_edit_iter->_p_section_iter);
//    if (NULL!=p_edit_iter->_p_edit_iter) {
//        dynamic_array_iter_free(p_edit_iter->_p_edit_iter);
//    };
//}

static censor_edit_node_t * censor_edit_iter_next( censor_edit_iter_t *p_edit_iter) {
    if ( NULL == p_edit_iter->_p_edit_iter ) {
        censor_edit_section_t *p_section;
        p_section = (censor_edit_section_t *)dynamic_array_iter_next(p_edit_iter->_p_section_iter);
        if ( NULL == p_section) {
            return NULL;
        }
        p_edit_iter->_p_edit_iter = dynamic_array_iter_new(p_section->p_edits);
    }
    censor_edit_node_t *p_edit;
    p_edit = (censor_edit_node_t *)dynamic_array_iter_next(p_edit_iter->_p_edit_iter);
    if ( NULL == p_edit) {
        free(p_edit_iter->_p_edit_iter);
        p_edit_iter->_p_edit_iter = NULL;
        return censor_edit_iter_next(p_edit_iter);
    } else {
        return p_edit;
    }
}

static void print_edit( censor_edit_node_t *p_edit ) {
    fprintf( stderr, "    EDIT_NODE\n");
    fprintf( stderr, "        start_time: %d.%d\n", (int)p_edit->p_start_time->tv_sec,(int)p_edit->p_start_time->tv_usec);
    fprintf( stderr, "        end_time  : %d.%d\n", (int)p_edit->p_end_time->tv_sec,  (int)p_edit->p_end_time->tv_usec);
    edit_t edit = p_edit->edit;
    fprintf( stderr, "        edit      : %s\n", edit == SKIP_OP ? "SKIP" : (edit==MUTE ? "MUTE" : "DIM"));
    
}

static void dump_edit_collection( censor_edit_collection_t *p_edit_collection ) {
    censor_edit_iter_t *p_edit_iter;
    p_edit_iter = censor_edit_iter_new(p_edit_collection);
    censor_edit_node_t *p_edit;
    while ( (p_edit = censor_edit_iter_next(p_edit_iter)) ) {
        print_edit(p_edit);
    }
}

static void print_edits( dynamic_array_t *pda_censor_edit_nodes ) {
    dynamic_array_iter_t * p_iter = dynamic_array_iter_new(pda_censor_edit_nodes);
    censor_edit_node_t *p_edit;
    while ( (p_edit = (censor_edit_node_t *)dynamic_array_iter_next(p_iter)) ) {
        print_edit(p_edit);
    }
    dynamic_array_iter_free(p_iter);
}



static int compare_timevals( struct timeval *p_a, struct timeval *p_b ) {
    if ( p_a == NULL ) 
        fprintf( stderr, "p_a == NULL\n");
    if ( p_b == NULL ) 
        fprintf( stderr, "p_b == NULL\n");
    if (p_a->tv_sec < p_b->tv_sec ||
        (p_a->tv_sec == p_b->tv_sec &&
        p_a->tv_usec < p_b->tv_usec)) 
    {
        return LESS_THAN;
    }
    
    if (p_b->tv_sec < p_a->tv_sec ||
        (p_b->tv_sec == p_a->tv_sec &&
        p_b->tv_usec < p_a->tv_usec)) 
    {
        return GREATER_THAN;
    }
    
    return EQUAL;
}

static dynamic_array_t * intersecting_edits( censor_edit_collection_t *p_edit_collection, struct timeval *t ) {
    
    dynamic_array_t * pda_edits;
    pda_edits = new_dynamic_array(8);
    
    censor_edit_iter_t *p_edit_iter;
    p_edit_iter = censor_edit_iter_new(p_edit_collection);
    censor_edit_node_t *p_edit;
    while ( (p_edit = censor_edit_iter_next(p_edit_iter)) ) {
        if (compare_timevals(p_edit->p_start_time,t) != GREATER_THAN &&
            compare_timevals(p_edit->p_end_time,t) != LESS_THAN )
        {
            dynamic_array_append(pda_edits,p_edit);
        }
    }
    
    return pda_edits;
}



static struct timeval *absolute_time_to_next_edit( censor_edit_collection_t *p_edit_collection, struct timeval *p_movie_time) {
    
    struct timeval *p_next_edit_time;
    p_next_edit_time = 0;
    
    censor_edit_iter_t *p_edit_iter;
    p_edit_iter = censor_edit_iter_new(p_edit_collection);
    censor_edit_node_t *p_edit;
    while ( (p_edit = censor_edit_iter_next(p_edit_iter)) ) {
        if ( compare_timevals(p_edit->p_start_time,p_movie_time) == GREATER_THAN &&
            (p_next_edit_time == NULL ||
            compare_timevals(p_edit->p_start_time,p_next_edit_time) == LESS_THAN) )
        {
            p_next_edit_time = p_edit->p_start_time;
            //fprintf( stderr, "New next edit time: %d\n", p_next_edit_time->tv_sec);
        }
                                                                                                  
        if ( compare_timevals(p_edit->p_end_time,p_movie_time) == GREATER_THAN &&
            (p_next_edit_time == NULL ||
            compare_timevals(p_edit->p_end_time,p_next_edit_time) == LESS_THAN ))
        {
            p_next_edit_time = p_edit->p_end_time;
            //fprintf( stderr, "New next edit time: %d\n", p_next_edit_time->tv_sec);
        }
    }
    
    return p_next_edit_time;
}
 
 
struct intf_sys_t
{
    input_thread_t *p_input;
};

static void Run( intf_thread_t *p_intf );

static void load_censor_file( intf_thread_t *p_intf, const char *filename, censor_edit_collection_t *p_edit_collection) 
{
    p_intf = p_intf;
    pANTLR3_UINT8 fName;
    pANTLR3_INPUT_STREAM input;
    pCensorLexer lxr;
    pANTLR3_COMMON_TOKEN_STREAM tstream;
    pCensorParser psr;
    
    fName = (pANTLR3_UINT8)filename;
    input  = antlr3AsciiFileStreamNew(fName);
    if ( input == NULL ) {
        fprintf(stderr, "Unable to open file %s due to malloc() failure1\n", (char *)fName);
    }
    
    lxr = CensorLexerNew(input);
    if ( lxr == NULL ) {
        fprintf(stderr, "Unable to create the lexer due to malloc() failure1\n");
        exit(ANTLR3_ERR_NOMEM);
    }
    
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, lxr->pLexer->tokSource);
    if (tstream == NULL) {
        fprintf(stderr, "Out of memory trying to allocate token stream\n");
        exit(ANTLR3_ERR_NOMEM);
    }
    
    psr = CensorParserNew(tstream);
    if (psr == NULL) {
        fprintf(stderr, "Out of memory trying to allocate parser\n");
        exit(ANTLR3_ERR_NOMEM);
    }
    p_edit_collection->p_censor_sections = psr->censorship_file(psr);


    psr     ->free  (psr);      psr     = NULL;
    tstream ->free  (tstream);  tstream = NULL;
    lxr     ->free  (lxr);      lxr     = NULL;
    input   ->close (input);    input   = NULL;

}

static int CensorActivate( vlc_object_t *p_this)
{
    intf_thread_t *p_intf = (intf_thread_t*)p_this;

    p_intf->p_sys = malloc( sizeof( intf_sys_t ) );
    if( !p_intf->p_sys )
    {
        msg_Err( p_intf, "no memory" );
        return VLC_ENOMEM;
    }

    p_intf->p_sys->p_input = NULL;
    p_intf->pf_run = Run;
    
    load_censor_file( p_intf, "/home/ross/vlc/modules/censor/test_input.cnsr",&edit_collection );
    
    return VLC_SUCCESS;
}


static void Run( intf_thread_t *p_intf )
{
    fprintf( stderr, "*Com-link online*\n");
    printf("*Com-link online*\n");
    config_PutPsz(p_intf, "hello_world", "who's your daddy");
    char *hello_world = config_GetPsz(p_intf, "hello_world");
    fprintf( stderr, "hello_world=%s",hello_world);
    perror("Here I am!\n");
                        
    struct timeval movie_time;
    //time_t cur_time = 0;
    movie_time.tv_usec = 0;
    
    movie_time.tv_sec = var_GetTime(p_intf->p_libvlc,"time");
                             
    fprintf( stderr, "All edits:\n");
    dump_edit_collection(&edit_collection);
    
    
    dynamic_array_t *pda_active_edits;
    
    pda_active_edits = intersecting_edits(&edit_collection, &movie_time);
    
    print_edits(pda_active_edits);
    
    struct timeval *p_next_edit_time, *p_last_edit_time;
    
    short previously_dimmed = 1;
    
    // Setup the socket for telling this plugin when a new file needs to be 
    // opened
    int censor_file_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == censor_file_socket_fd) {
        perror("ERROR opening censor file listening socket\n");
    }
    // try to bind to a random port multiple times
    struct sockaddr_in srvr_addr;
    srvr_addr.sin_family = AF_INET;
    srvr_addr.sin_addr.s_addr = INADDR_ANY;
    srand(time(NULL));
    bool bound_port = false;
    for( int i = 0; i < 100; i++ ) {
        srvr_addr.sin_port = rand()%(65535-10000) + 10000;
        if( bind(censor_file_socket_fd, (struct sockaddr *) &srvr_addr,
            sizeof(srvr_addr)) >= 0) {
            bound_port = true;
            break;
        }
    }
    if(!bound_port) {
        perror("ERROR couldn't bind censor file listening socket to a port after many attempts\n");
    }
    listen(censor_file_socket_fd,5);
    
    
    while (1) 
    {
        // poll for censor file input on the socket
        int poll_val = poll( &censor_file_socket_fd, 1, 0);
        if( poll_val > 0) {
            // Load the censor file
            
        }
        
        movie_time.tv_sec = var_GetTime(p_intf->p_libvlc,"time");
        if ( movie_time.tv_sec < 0 )
            movie_time.tv_sec = 0;
        fprintf( stderr, "Time : %d seconds\n", (int)movie_time.tv_sec);
        pda_active_edits = intersecting_edits(&edit_collection, &movie_time);
        print_edits(pda_active_edits);
        short dim_on = 0, mute_on = 1; // No flag for skip since that is the combo of the two
        
        dynamic_array_iter_t *p_active_edits_iter;
        p_active_edits_iter = dynamic_array_iter_new(pda_active_edits);
        censor_edit_node_t *p_active_edit; 
        
        while ( (!dim_on || !mute_on ) &&
                (p_active_edit = (censor_edit_node_t *)dynamic_array_iter_next(p_active_edits_iter))) {
        
            switch (p_active_edit->edit) {
              case SKIP_OP:
                dim_on  = 1;
                mute_on = 1;
                break;
              case MUTE:
                mute_on = 1;
                break;
              case DIM:
                dim_on  = 1;
                break;
              default :
                fprintf(stderr,"ERROR: Unknown edit type for p_active_edit->edit");
                break;
            }
        }
        dynamic_array_iter_free(p_active_edits_iter); p_active_edits_iter = NULL;
        
        
        
        p_next_edit_time = absolute_time_to_next_edit(&edit_collection, &movie_time);
        if ( p_next_edit_time !=NULL && p_next_edit_time != p_last_edit_time )
            fprintf( stderr, "Next edit time : %d s\n", (int)p_next_edit_time->tv_sec);
        p_last_edit_time = p_next_edit_time;
        
        if ( dim_on && mute_on ) {
            var_SetTime(
                p_intf->p_libvlc,
                "time",
                p_next_edit_time->tv_sec);
            
        } else if ( mute_on ) {
            audio_volume_t cur_volume;
            aout_VolumeGet(p_intf->p_libvlc,&cur_volume);
            if (AOUT_VOLUME_MIN != cur_volume) {
                aout_ToggleMute(p_intf->p_libvlc, NULL);
            }
        } else if (!mute_on) {
            audio_volume_t cur_volume;
            aout_VolumeGet(p_intf->p_libvlc,&cur_volume);
            if (AOUT_VOLUME_MIN == cur_volume) {
                aout_ToggleMute(p_intf->p_libvlc, NULL);
            }
        } else if ( dim_on && !previously_dimmed) {
            previously_dimmed = dim_on;
            // TODO
        } else if ( !dim_on && previously_dimmed) {
            previously_dimmed = dim_on;
            // TODO
        }
        
        
        int sleep_time = 2;
        if ( p_next_edit_time != NULL ) 
            sleep_time =(p_next_edit_time->tv_sec-movie_time.tv_sec);
        
        if ( sleep_time > 3 )
            sleep_time = 3;
        time_t wake_time = time(NULL)+sleep_time;
        
        fprintf( stderr, "Sleep time : %d s\n\n", sleep_time);
        
        while( time(NULL) < wake_time ) { 
            sleep(1);
        }
    }
}
static void CensorClose( vlc_object_t *arg){ 
    arg = arg;
    dead_code();
}
//static void Thread   ( vlc_object_t *arg) { }
 
//void hello_world( libvlc_int_t * p_libvlc ) 
//{
//    fprintf( stderr, "HELLO WORLD!");
//}


