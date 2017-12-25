/*
File that contains the game logic
Written by Dag Hjelm and Joar Rutqvist 2017
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"

//Height and width of the screen
#define HEIGHT 32
#define WIDTH 128
//Height and width of the ship
#define SHIPH 10
#define SHIPW 3
//Height and width of the enemies/obstacles
#define ENEMYH 3
#define ENEMYW 3
//Number of enemies/obstacles
#define NROFENS 5

//These are the ships starting positions.
//We will use these global variables to know the x and y position of the
//Ship
double xpos = 0;
double ypos = 0;
//This is array which we render to the display
uint8_t screen[128*4] = {0};

//score counter :)
int score = 0;
//First and second high score
int scoreOne = 0;
int scoreTwo = 0;
//boolean for the menu
int checking = 0;

int numberofenemies = NROFENS;

int enemies[NROFENS][2] = {0}; //Varför funkar det inte att använda #define värden eller
						//globala variabler för att initiera arrayn?
//int enemies[NROFENS][2] = {0};

//for the rand() function
int seed = 123456789;

//Returns a (not even) pseudo random number between 0 and 9
int rand() {
	int m = 872346;
	int a = 123859;
	int c = 632876;

	seed = (a * seed + c) % m;

	int r;

	if(seed < 0) {
		r = seed * -1;
	} else {
		r = seed;
	}

	return r % 10;
}

//converts an x,y position in the normal way to the SPI way
//and puts it in the array so we can easily draw stuff
void addPixel(int x, int y, uint8_t *array) {
	int h = HEIGHT;
	int w = WIDTH;
	//So that it wont add pixels in positions that are not allowed.
	if(x <= 0 || x >= w || y <= 0 || y >= h) {
		return;
	}
	//In which of the four rows is the pixel located?
	//Assuming that y > 0 and y < 32 this will always be an int between 0 and 3.
    int row = y / 8;

	//offset * 128 gives us the right row, + x gives ut the right x position.
	//y - offset * 8 gives us the y-position in the row.
    array[row * 128 + x] |= (1 << (y - row * 8));
}

//draw the ship at current position
void drawShip() {
    int h = SHIPH;
    int w = SHIPW;
    int i, j;

    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            addPixel((int) xpos + j,(int) ypos + i, screen);
        }
    }
}

//draw an enemy/obstacle at set position
void drawE(int x, int y) {

    int h = ENEMYH;
    int w = ENEMYW;
    int i, j;

    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            addPixel(x + j, y + i, screen);
        }
    }
}

//add an ememy to the enemies array.
void addEnemy(int x, int y, int nr) {
    enemies[nr][0] = x;
    enemies[nr][1] = y;
}

//Fill the array enemies array with good starting positions.
void fillenemies() {
	int i;
	int w = WIDTH;
	int ew = ENEMYW;
	for(i = 0; i < numberofenemies; i++) {
		//add enemy at far right x position and random y position.
		//TODO smart lösning så att alla börjar på en najs position.
		addEnemy(w - ew + 25 * i, 3 * rand(), i);
	}
}

//Check if an enemy has gone out of bounds.
//If it has - reset i to start at the beggining.
void checkenemies() {
	int i;
	int ew = ENEMYW;
	int sw = SHIPW;
	for(i = 0; i < numberofenemies; i++) {
		if(enemies[i][0] <= 0 - sw) {
			enemies[i][0] = WIDTH;
			enemies[i][1] = 3 * rand();
		}
	}
}

//update the position of every enemy with s pixel(s)
void updateenemies(int s) {
	int i;
	for(i = 0; i < numberofenemies; i++) {
		enemies[i][0] = enemies[i][0] - s;
	}
}

//draw all enemies
void drawenemies() {
	int i;

	for(i = 0; i < numberofenemies; i++) {
		int x = enemies[i][0];
		int y = enemies[i][1];

		drawE(x, y);
	}
}

//See which button is pressed
int getbtn(int btn) {
	int value = 0;
	switch (btn) {
		case 1:
			value = (PORTF)&2;
			break;
		case 2:
			value = ((PORTD>>5)&7) & 1;
			break;
		case 3:
			value = ((PORTD>>5)&7) & 2;
			break;
		case 4:
			value = ((PORTD>>5)&7) & 4;
			break;
		default:
			value = 0;
	}
	return value;
}

//clear screen, set every byte to 0
void clear() {
	int i;
	for(i = 0; i < sizeof(screen); i++) {
		screen[i] = 0;
	}
}

#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}

int collisions() {
	int i;
	int enemywidth = ENEMYW;
	int enemyheight = ENEMYH;
	int shipwidth = SHIPW;
	int shipheight = SHIPH;


	for(i = 0; i < numberofenemies; i++) {
		int eLeft = enemies[i][0];
		int eTop = enemies[i][1];
		int eRight = eLeft + enemywidth;
		int eBottom = eTop + enemyheight;

		int shipLeft = xpos;
		int shipTop = ypos;
		int shipRight = xpos + shipwidth;
		int shipBottom = ypos + shipheight;

		if (
			eLeft <= shipRight &&
			eRight >= shipLeft &&
			eTop <= shipBottom &&
			eBottom >= shipTop
		) {
			return 1;
		}
	}

	return 0;
}

int strlen(char *str) {
	int i;
	for(i = 0; str[i] != '\0'; i++);
	return i;
 }

char * stringconcat(char *str1, char *str2) {
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int both = len1 + len2;

	char newstring[both];
	int i;
	for(i = 0; i < len1; i++) {
		newstring[i] = str1[i];
	}
	for(i = 0; i < len2; i++) {
		newstring[i+len1] = str2[i];
	}

	return newstring;
}

void gameOver() {
		ledAnimationTwo();
		if(score > scoreOne) {
			scoreTwo = scoreOne;
			scoreOne = score;
		}
		if(score < scoreOne && score > scoreTwo) {
			scoreTwo = score;
		}
		int gameover = 1;
		display_string(0, "Game Over! :(");

		//char *score_string = itoaconv(score);
		char *line = stringconcat("Score: ", itoaconv(score));

		display_string(1, line);
		display_string(2, "Press BTN3");
		display_string(3, "to play again!");
		display_update();

		xpos = 0;
		ypos = 0;
		score = 0;

		while(gameover) {
		    if(getbtn(3)) {
				gameover = 0;
		}
	}
}

void moveship() {

    int h = SHIPH;
    int w = SHIPW;

	if(getbtn(2)) {
        if(ypos >= 0)
        ypos -= 0.25;
    }
    if(getbtn(3)) {
        if(ypos <= (31-h))
        ypos += 0.25;
    }
    if(getbtn(1)) {
        if(xpos <= (127-w))
        xpos += 0.25;
    }
    if(getbtn(4)) {
        if(xpos >= 0)
        xpos -= 0.25;
    }
}


//Bulk of the game, the main game loop is here.
void play() {
	score = 0;
	int playing = 1;
	int period = 15;
	int periodcounter = 0;
	int timeoutcounter = 0;
	int scoreperiod = 0;
	int speed = 1;

	//TODO maybe replace with constants instead of using #define?
	int screenwidth = WIDTH;
	int screenheight = HEIGHT;

	int enemyw = ENEMYW;
	int enemyh = ENEMYH;

	fillenemies();

	while(playing) {

		moveship();

	    clear();

		if(IFS(0) & 0x100) {
			timeoutcounter++;
			periodcounter++;
			scoreperiod++;
			IFSCLR(0) = 0x100;
		}

		if(timeoutcounter == period) {
			updateenemies(speed);
			timeoutcounter = 0;
		}

		if(periodcounter == 2000) {
			if(period > 2) {
				period = period - 1;
			}
			periodcounter = 0;
		}

		if(scoreperiod == 100) {
			score++;
			PORTE += 1;
			scoreperiod = 0;
		}


		checkenemies();
		drawenemies();
    	drawShip();
    	render(screen);
		//While loop stops if we have a collision
		playing = !collisions();
	}
}

void start() {

	int running = 1;
	int counter = 0;
	int ledcounter = 0;

	display_string(0, "****************");
	display_string(1, "---SPACERACER---");
	display_string(2, "BTN4: Start");
	display_string(3, "BTN1: High Score");
	display_update();

	int i;
	ledAnimationOne();
	ledAnimationOne();
	ledAnimationThree();


	while(running) {
		if(getbtn(4)) {
			running = 0;
		}

		if(getbtn(1)) {
			running = 0;
			checking = 1;
		}
	}
}

void highScore() {

	clearbuffer();
	display_update();

	display_string(0, "--HIGH SCORES--");
	display_string(2, stringconcat("2. ", itoaconv(scoreTwo)));
	display_string(1, stringconcat("1. ", itoaconv(scoreOne)));
	display_string(3, "Start: BTN3");
	display_update();

	while(1) {
		if(getbtn(3)) {
			break;
		}
	}

	checking = 0;
}

//Infinite loop that switches between gameover screen and the game.
void run() {
	while (1) {
		start();
		if(checking) {
			highScore();
		}
		play();
		gameOver();
	}
}
