#ifndef INC_HMI_H_
#define INC_HMI_H_

#include "usart.h"
#include "battery_show.h"
#include "SD_APP.h"
#include "ff.h"
#include "dosage.h"

#define MAX_DISPLAY_ITEMS 6		//每页最多显示6行

enum HMI_PAGE {Main_page,File_M_page};
enum CONFIRM_MSG {CONFIRM_NULL,CONFIRM_SEND_FILE,CONFIRM_DELETE_FILE};
enum CONFIRM_RESAULT {CANCEL_CONFIRM,DELETE_FILE,SEND_FILE};
// 分页控制结构体
typedef struct {
    uint16_t file_count;
    uint16_t start_index;
    uint8_t current_page;
    uint8_t total_pages;
    char** file_list;
    uint8_t show_prev_more; // 新增：是否显示前页更多
    uint8_t show_next_more; // 新增：是否显示后页更多
} PagingState;

//// 使用LRU缓存最近访问的目录页
//#define CACHE_PAGES 3
//struct {
//    uint16_t start;
//    char* files[MAX_DISPLAY_ITEMS];
//} page_cache[CACHE_PAGES];

typedef struct{
	uint8_t display;
	enum CONFIRM_MSG type;
}MsgState;

extern uint8_t dir_display_refresh;
extern uint8_t refresh_dir;
extern uint8_t first_display_dir;
extern uint8_t current_focus_line;// 当前焦点行0-5
extern uint8_t last_focus_line;
extern uint8_t focus_key_pressed;
extern enum CONFIRM_RESAULT sec_confirm_resault;
extern enum HMI_PAGE page_location;

void HMI_init(void);
void scroll_focus_line(void);
void Refresh_Display();
void Cache_File_List();
void On_Key_Pressed(uint8_t key_code);
void Back_to_MainPage(void);
void page_turning(void);

#endif /* INC_HMI_H_ */
