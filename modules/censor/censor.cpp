
#define __LIBVLC__ 1
#include "censor.hpp"
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_interface.h>
#include <vlc_aout.h>
//#include <vlc_fixups.h>
//#include "CensorParser.h"
//#include "CensorLexer.h"
#include <sys/poll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <expat.h>
/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
  
vlc_module_begin();
    set_shortname("Censor");
    set_description("auto-censor module");
    set_category(CAT_ADVANCED );
    set_subcategory(SUBCAT_ADVANCED_MISC );
    set_callbacks(CensorActivate, CensorClose);
    set_capability("interface",0);
    add_shortcut("censor" );
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


void CnsrEdit::print() {
    fprintf( stderr, "    EDIT_NODE\n");
    fprintf( stderr, "        start_time: %d.%d\n", (int)startTime.tv_sec,(int)startTime.tv_usec);
    fprintf( stderr, "        end_time  : %d.%d\n", (int)endTime.tv_sec,  (int)endTime.tv_usec);
    char *actionStr = "UNDEF";
    switch (cnsrEditAction) {
      case SKIP: actionStr = "SKIP"; break;
      case MUTE: actionStr = "MUTE"; break;
      case DIM:  actionStr = "DIM";  break;
    }
    fprintf(stderr, "        edit      : %s\n", actionStr);
}

void CnsrEditcollection::print() {
    for (vector<CnsrEdit>::iterator it = edits.begin(); it != edits.end(); ++it) {
        it->print();
    }
}
f
static int compareTimevals( struct timeval &b, struct timeval &b) {
    if ( a == NULL ) 
        fprintf( stderr, "a == NULL\n");
    if ( b == NULL ) 
        fprintf( stderr, "b == NULL\n");
    if (a.tv_sec < b.tv_sec ||
        (a.tv_sec == b.tv_sec &&
        a.tv_usec < b.tv_usec)) 
    {
        return LESS_THAN;
    }
    
    if (b.tv_sec < a.tv_sec ||
        (b.tv_sec == a.tv_sec &&
        b.tv_usec < a.tv_usec)) 
    {
        return GREATER_THAN;
    }
    
    return EQUAL;
}

vector<CnsrEdit>* CnsrEditCollection::intersectingEdits(struct timeval &t) {
    vector<CnsrEdit> *result = new vector<CnsrEdit>;
    for (vector<CnsrEdit>::iterator it = edits.begin(); it != edits.end(); ++it) {
        if (compareTimevals(it->startTime,t) != GREATER_THAN &&
            compareTimevals(it->endTime,t) != LESS_THAN ) {
            result.push_back(it);
        }
    }
    return result;
}



struct timeval *CnsrEditCollection::timeToNextEdit(struct timeval &curTime);
    
    struct timeval *nextEditTime = NULL;
    
    for (vector<CnsrEdit>::iterator it = edits.begin(); it != edits.end(); ++it) {
        if (compareTimevals(it->startTime,curTime) == GREATER_THAN &&
            (nextEditTime != NULL ||
            compareTimevals(it->startTime,*nextEditTime) == LESS_THAN)) {
            nextEditTime = &it->startTime;
            //fprintf( stderr, "New next edit time: %d\n", nextEditTime->tv_sec);
        }
                                                                                                  
        if ( compareTimevals(it->endTime,curTime) == GREATER_THAN &&
            (nextEditTime == NULL ||
            compareTimevals(it->endTime,*nextEditTime) == LESS_THAN ))
        {
            nextEditTime = &it->endTime;
            //fprintf( stderr, "New next edit time: %d\n", nextEditTime->tv_sec);
        }
    }
    
    return nextEditTime;
}
 
 
struct intf_sys_t
{
    input_thread_t *p_input;
};

static CnsrEditCollection *CnsrEditCollection::inst() {
    if (NULL == _inst)
        _inst = new CnsrEditCollection();
    return _inst;
}

// XML parsing
static void XMLCALL
beginXmlTag(void *data, const char *el, const char **attr) {
    CnsrEditAction action = EDIT_UNDEF;
    if (0==strcasecmp(el,"skip")) {
        action = SKIP;
    } else if (0==strcasecmp(el,"mute")) {
        action = MUTE;
    } else if (0==strcasecmp(el,"dim")) {
        action = DIM;
    } else {
        return;
    }
    CnsrEdit *edit = new CnsrEdit;
    edit->cnsrEditAction = action;
    for (int i = 0; attr[i]; i+=2) {
        if (0==strcasecmp(attr[i],"begin")) {
            edit->startTime = parseTime(attr[i+1]);
        } else if (0==strcasecmp(attr[i],"begin")) {
            edit->endTime = parseTime(attr[i+1]);
        } else if (0==strcasecmp(attr[i],"ratings")) {
            parseRatings(edit->editClassifications,attr[i+1]);
        }
    }
}

static struct timeval parseTime(char *timeStr) {
    // form h:m:s.s
    char *begTok = timeStr;
    char *endTok = begTok;
    struct timeval result;
    result.tv_sec = 0;
    restul.tv_usec = 0;
    int numToks = 0;
    bool foundDot = false;
    while (true) {
        if ('.' == endTok) {
            foundDot = true;
        } else if ('\0' == *endTok || ':' == *endTok) {
            char originalEndTokChar = *endTok;
            *endTok = '\0';
            if (foundDot) {
                float seconds = atof(begTok);
                result.tv_sec = (result.tv_sec*60) + (int)seconds;
                result.tv_usec = (int)(1000*seconds);
                break;
            } else {
                int units = atoi(begTok);
                result.tv_sec = (result.tv_sec*60) + (int)units;
            }
            
            numToks++;
            if (numToks >= 3 || '\0' == originalEndTokChar) {
                break;
            } else {
                endTok++;
                begTok = endTok;
                continue;
            }
        }
        
        endTok++;
    }
    return result;
}

static void parseRatings(vector<CnsrEditClassification> &editClassifications, char *ratingsStr) {
    // form ((G|PG|PG-13|R)(:/hier1(/hier2)*)?),[etc]
    char *begTok = ratingsStr;
    while ('\0' != *ratingStr) {
        CnsrEditClassification *classification = new CnsrEditClassification;
        classification->rating = RATING_UNDEF;
        while (':' != *ratingsStr && '\0' != *ratingsStr && ',' != *ratingsStr) {
            ratingsStr++;
        }
        char origChar = *ratingsStr;
        *ratingsStr = '\0';
        if (0 == strcasecmp(ratingsStr,"G")) {
            classification->rating = G;
        } else if (0 == strcasecmp(ratingsStr,"PG13") || 0 == strcasecmp(ratingsStr,"PG-13")) {
            classification->rating = PG13;
        } else if (0 == strcasecmp(ratingsStr,"PG")) {
            classification->rating = PG;
        } else if (0 == strcasecmp(ratingsStr,"R")) {
            classification->rating = R;
        }
        *ratingsStr = origChar;
        if (':' == *ratingsStr) {
            ratingsStr++;
            begTok = ratingsStr;
            while ('\0' != *ratingsStr && ',' != *ratingsStr) {
                if ('/' == *ratingsStr) {
                    origChar = *ratingsStr;
                    *ratingsStr = '\0';
                    if (begTok != ratingsStr) {
                        char *hierLvl = new char[strlen(begTok)+1];
                        strcpy(hierLvl, begTok);
                        classification->categoryHierarchy.push_back(hierLvl);
                        
                    }
                    *ratingsStr = origChar;
                }
            }
        }
    }
    
}




fstatic void loadCensorFile( intf_thread_t *p_intf, const char *filename)
{
    
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

static CnsrEditCollection::_inst = NULL;
//static void Thread   ( vlc_object_t *arg) { }
 
//void hello_world( libvlc_int_t * p_libvlc ) 
//{
//    fprintf( stderr, "HELLO WORLD!");
//}

