/*
Header file for the game.
Written by Dag Hjelm and Joar Rutqvist
*/
void configbtn(void);
void configscreen(void);
void user_isr(void);
uint8_t spi_send_recv(uint8_t data);
void display_init(void);
void display_update(void);
void display_string(int line, char *s);
extern const uint8_t const font[128*8];
extern char textbuffer[4][16];
void addPixel(int x, int y, uint8_t *array);
void clear();
void play(void);
void fillEnemys(void);
void configtimer(void);
void run(void);
void configLed(void);
void ledAnimationOne(void);
void ledAnimationTwo(void);
void ledAnimationThree(void);
void clearbuffer(void);
