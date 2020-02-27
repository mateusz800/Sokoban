#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>


extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	600
#define IMAGE_SIZE 50
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 50
#define TRUE 1
#define FALSE 0



#define SPEED 1.5 //predkosc poruszania sie gracza

#define MAIN_MENU 1
#define NEXT_LEVEL_MENU 2

enum field{
	WALL=1,
	FLOOR,
	CHARACTER,
	BOX,
	GOAL
};
enum move {
	RIGHT,
	LEFT,
	UP,
	DOWN,
	NONE
};
enum button {
	MAIN_MENU_BTN,
	NEXT_LEVEL_BTN
};


typedef struct map_t {
	SDL_Surface *wall;
	SDL_Surface *floor;
	SDL_Surface *box;
	SDL_Surface *goal;
	SDL_Surface *characterUp;
	SDL_Surface *characterDown;
	SDL_Surface *characterLeft;
	SDL_Surface *characterRight;
	SDL_Surface *logo;
	int lastDirection ;
};


// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};
void initBoard(char*source, int size, int*board ){
	FILE * file;
	file = fopen(source, "r");

	for (int i = 0; i < size; i++, board++) {
		
		fscanf_s(file, "%d", board);
		if (board[i] == EOF)
			break;
	}
	fclose(file);
	
}
map_t loadImages() {
	map_t map;
	map.box = SDL_LoadBMP("img/box.bmp");
	map.floor = SDL_LoadBMP("img/floor.bmp");
	map.goal = SDL_LoadBMP("img/goal.bmp");
	map.wall = SDL_LoadBMP("img/wall.bmp");
	map.characterUp = SDL_LoadBMP("img/characterUp.bmp");
	map.characterRight = SDL_LoadBMP("img/characterRight.bmp");
	map.characterLeft = SDL_LoadBMP("img/characterLeft.bmp");
	map.characterDown = SDL_LoadBMP("img/characterDown.bmp");
	map.logo = SDL_LoadBMP("img/logo.bmp");
	///przezroczystosc gracza
	SDL_SetColorKey(map.characterUp, 1, SDL_MapRGB(map.characterUp->format, 255, 255, 255));
	SDL_SetColorKey(map.characterDown, 1, SDL_MapRGB(map.characterDown->format, 255, 255, 255));
	SDL_SetColorKey(map.characterLeft, 1, SDL_MapRGB(map.characterLeft->format, 255, 255, 255));
	SDL_SetColorKey(map.characterRight, 1, SDL_MapRGB(map.characterRight->format, 255, 255, 255));
	SDL_SetColorKey(map.logo, 1, SDL_MapRGB(map.logo->format, 255, 255, 255));
	return map;
}
int* loadBoard(int index,int * board,int* sizeX,int *sizeY) {
	FILE * file;
	file = fopen("boards.txt", "r");
	char source [19];
	char *temp=(char*)malloc(20*sizeof(char));
	int mapNumber = 1;
	int i = 0;
	for (int j = 0; j < index; j++) {
		 i = -1;
		while (*temp != ',') {
			if(i>-1)
				source[i] = *temp;
			i++;
			if(mapNumber==index)
				temp++;
			fscanf_s(file, "%c", temp);
		}
		temp++;
		fscanf_s(file, "%d", sizeX);
		fscanf_s(file, "%d", sizeY);
		mapNumber++;
	}	
	fclose(file);
	source[18] = '\0';
	
	free(board);
	int*newBoard = (int*)malloc(*sizeX*(*sizeY) * sizeof(int));
	initBoard(source,(*sizeX)*(*sizeY), newBoard);
	
	return newBoard;

}

int getCharacterIndex(int* board, int size) {
	for (int i = 0; i < size; i++) {
		if (board[i] == CHARACTER)
			return i;
	}
	return -1;
}


void drawBoard(SDL_Surface *screen,int * board,int sizeX,int sizeY,map_t map,double characterX,double characterY,int animateBox) {
	int boardX = (SCREEN_WIDTH - sizeX*IMAGE_SIZE) / 2; //wspolrzedna x planszy (srodek)
	int boardY = (SCREEN_HEIGHT - sizeY*IMAGE_SIZE) / 2; //wspolrzedna y planszy (srodek)
	int animate = LEFT;
	int index = getCharacterIndex(board, sizeX*sizeY);

	DrawSurface(screen, map.floor, index%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, index / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
	for (int i = 0; i <sizeX*sizeY; i++) {
		switch (board[i]) {
		case WALL:
			DrawSurface(screen, map.wall, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2+boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 +boardY);
			break;
	
		case GOAL:
			DrawSurface(screen, map.goal, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
			break;
		case FLOOR:
			DrawSurface(screen, map.floor, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
			break;
		case CHARACTER:
			//DrawSurface(screen, map.floor, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
			break;
		case BOX:
			DrawSurface(screen, map.floor, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
			if (board[i - 1] == CHARACTER && i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY == characterY&&animateBox == RIGHT) //ruch skrzynki w prawo
				DrawSurface(screen, map.box, characterX + IMAGE_SIZE, characterY);
			else if (board[i + 1] == CHARACTER && i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY == characterY&&animateBox == LEFT)
				DrawSurface(screen, map.box, characterX - IMAGE_SIZE, characterY);
			else if (board[i + sizeX] == CHARACTER && i % sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX  == characterX && animateBox == UP)//ruch skrzynki w gore
				DrawSurface(screen, map.box, characterX, characterY - IMAGE_SIZE);
			else if (board[i - sizeX] == CHARACTER && i % sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX == characterX && animateBox == DOWN)//ruch skrzynki w dol
				DrawSurface(screen, map.box, characterX, characterY +IMAGE_SIZE);
			else
				DrawSurface(screen, map.box, i%sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardX, i / sizeX*IMAGE_SIZE + IMAGE_SIZE / 2 + boardY);
			break;
	
		}
		DrawSurface(screen, map.wall, 100, 100);
	}
	switch (map.lastDirection) {
	case UP:
		DrawSurface(screen, map.characterUp, characterX, characterY);
		break;
	case DOWN:
		DrawSurface(screen, map.characterDown, characterX, characterY);
		break;
	case LEFT:
		DrawSurface(screen, map.characterLeft, characterX, characterY);
		break;
	case RIGHT:
		DrawSurface(screen, map.characterRight, characterX, characterY);
		break;

	}
		
	
}

int *createTempBoard(int*board, int sizeX, int sizeY) {
	int * tab = (int*)malloc(sizeX*sizeY * sizeof(int));
	
	for (int i = 0; i < sizeX*sizeY; i++) {
		if (board[i] == GOAL)
			tab[i] = GOAL;
		else
			tab[i] = FLOOR;
	}
	return tab;
}


void moveCharacter(int * board, int move,int sizeX, int sizeY,int* animateBox,int * tempBoard) {
	int characterIndex = getCharacterIndex(board, sizeX*sizeY);
	switch (move) {
	case RIGHT:
		if (board[characterIndex + 1] == BOX && (board[characterIndex + 2] == FLOOR || board[characterIndex + 2] == GOAL)) {
			tempBoard[characterIndex + 2] = board[characterIndex + 2];
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex + 1] = CHARACTER;
			board[characterIndex + 2] = BOX;
			*animateBox = RIGHT;

		}
		else if (board[characterIndex + 1] != WALL &&board[characterIndex + 1] != BOX) {
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex + 1] = CHARACTER;
			*animateBox = NONE;
		}
		break;
	case LEFT:
		if (board[characterIndex - 1] == BOX && (board[characterIndex - 2] == FLOOR || board[characterIndex - 2] == GOAL)) {
			tempBoard[characterIndex - 2] = board[characterIndex - 2];
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex - 1] = CHARACTER;
			board[characterIndex - 2] = BOX;
			*animateBox = LEFT;

		}
		else if (board[characterIndex - 1] != WALL &&board[characterIndex - 1] != BOX) {
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex - 1] = CHARACTER;
			*animateBox = NONE;
		}
		break;
	case UP:
		if (board[characterIndex - sizeX] == BOX && (board[characterIndex - 2*sizeX] == FLOOR || board[characterIndex - 2*sizeX] == GOAL)) {
			tempBoard[characterIndex - 2*sizeX] = board[characterIndex - 2*sizeX];
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex - sizeX] = CHARACTER;
			board[characterIndex - 2*sizeX] = BOX;
			*animateBox = UP;

		}
		else if (board[characterIndex - sizeX] != WALL &&board[characterIndex - sizeX] != BOX) {
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex - sizeX] = CHARACTER;
			*animateBox = NONE;
		}
		break;
	case DOWN:
		if (board[characterIndex + sizeX] == BOX && (board[characterIndex + 2 * sizeX] == FLOOR || board[characterIndex + 2 * sizeX] == GOAL)) {
			tempBoard[characterIndex + 2 * sizeX] = board[characterIndex + 2 * sizeX];
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex + sizeX] = CHARACTER;
			board[characterIndex +2 * sizeX] = BOX;
			*animateBox = DOWN;

		}
		else if (board[characterIndex + sizeX] != WALL &&board[characterIndex + sizeX] != BOX) {
			board[characterIndex] = tempBoard[characterIndex];
			board[characterIndex + sizeX] = CHARACTER;
			*animateBox = NONE;
		}
		break;
	}

}
int getMapsCount() {
	FILE* file;
	file = fopen("boards.txt", "r");
	char sign;
	int count = 0;
	while (fscanf_s(file, "%c",&sign)!=EOF) {
		if (sign == ',')
			count++;
	}
	fclose(file);
	return count;
}

void drawButtons(SDL_Surface*screen, SDL_Surface * charset,int activeButton,int mapsCount) {
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0xFF);
	int blue = SDL_MapRGB(screen->format, 0x01, 0x99 ,0x99);
	char number[20];
	int y = 200;
	int x = BUTTON_WIDTH;
	int buttonsInRow = (SCREEN_WIDTH-2*BUTTON_WIDTH) / BUTTON_WIDTH;
	int row = 1;
	for (int i = 1; i <=mapsCount; i++) {
		if (i-(buttonsInRow*row)>= row) {
			y += BUTTON_HEIGHT;
			row++;
			x = BUTTON_WIDTH;
		}
		
		SDL_itoa(i, number, 10);
		if(activeButton==i)
			DrawRectangle(screen, x, y, BUTTON_WIDTH, BUTTON_HEIGHT, red,green);
		else
			DrawRectangle(screen, x, y, BUTTON_WIDTH, BUTTON_HEIGHT, red, blue);
		DrawString(screen, x + BUTTON_WIDTH / 2 - 4, y + BUTTON_HEIGHT / 2 - 4, number, charset);
		x += BUTTON_WIDTH;
	}
}
void showMenu(SDL_Surface * screen,SDL_Surface * charset,int activeButton,int mapsCount,map_t images) {
	int gray = SDL_MapRGB(screen->format, 0x22, 0x22, 0x022);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xEE);
	DrawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		blue, gray);
	
	DrawSurface(screen, images.logo, SCREEN_WIDTH/2, SCREEN_HEIGHT/5);
	drawButtons(screen, charset,activeButton,mapsCount);
}

int endOfGame(int * board, int size) {
	for (int i = 0; i < size; i++) {
		if (board[i] == GOAL)
			return FALSE;
	}
	return TRUE;
}
void drawTime(double time,SDL_Surface* charset,SDL_Surface * screen) {

	char text[128];
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	sprintf(text, " czas trwania = %.1lf s", time);
	DrawRectangle(screen, 4, 4, 4 + strlen(text) * 10, 36, czerwony, niebieski);
	DrawString(screen, 10, 10, text, charset);
}
void showNextLevelMenu(SDL_Surface* screen, SDL_Surface*charset,int activeButton,double time,int currentMap) {
	int gray = SDL_MapRGB(screen->format, 0x22, 0x22, 0x022);
	int blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xEE);
	int light_blue = SDL_MapRGB(screen->format, 0x00, 0xFF, 0xFF);
	DrawRectangle(screen, SCREEN_WIDTH /8 , SCREEN_HEIGHT / 8, SCREEN_WIDTH-SCREEN_WIDTH/4, SCREEN_HEIGHT-SCREEN_HEIGHT/4,blue, gray);

	//wyswietlanie czasu
	char timeStr[10];
	DrawString(screen, SCREEN_WIDTH / 4 + 2 * BUTTON_WIDTH, SCREEN_HEIGHT / 4, "Czas", charset);
	sprintf(timeStr, "  %.1lf s", time);
	DrawString(screen, SCREEN_WIDTH / 4 + 2 * BUTTON_WIDTH-10 , SCREEN_HEIGHT / 4 + 10, timeStr, charset);


	if (activeButton == MAIN_MENU_BTN) 
		DrawRectangle(screen, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, 2 * BUTTON_WIDTH, BUTTON_HEIGHT, gray, light_blue);
	else
		DrawRectangle(screen, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, 2 * BUTTON_WIDTH, BUTTON_HEIGHT, gray, blue);
	DrawString(screen, SCREEN_WIDTH / 4 +10, SCREEN_HEIGHT / 2 + BUTTON_HEIGHT / 2, "menu", charset);
	if(activeButton==NEXT_LEVEL_BTN && currentMap<getMapsCount())
		DrawRectangle(screen, SCREEN_WIDTH/4+2*BUTTON_WIDTH, SCREEN_HEIGHT/2, 2*BUTTON_WIDTH, BUTTON_HEIGHT, gray, light_blue);
	else if (currentMap < getMapsCount()) {
		DrawRectangle(screen, SCREEN_WIDTH / 4 + 2 * BUTTON_WIDTH, SCREEN_HEIGHT / 2, 2 * BUTTON_WIDTH, BUTTON_HEIGHT, gray, blue);
		DrawString(screen, SCREEN_WIDTH / 4 + 2 * BUTTON_WIDTH + 10, SCREEN_HEIGHT / 2 + BUTTON_HEIGHT / 2, "nastepny poziom", charset);
	}
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	int sizeX = 8;
	int sizeY = 8;
	int *board=(int*)malloc(sizeX*sizeY*sizeof(int));
	int * tempBoard=NULL;
	map_t map = loadImages();
	map.lastDirection = UP;

	int mapsCount = getMapsCount();//liczba dostepnych map
	int animate = NONE; //flaga czy animowac postac
	int animateBox = NONE;
	int menu = MAIN_MENU; //flaga czy menu jest wlaczone
	int activeMap = 1; //aktywny przycisk w menu
	int activeButton = MAIN_MENU_BTN;

	
	
	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Mateusz Budnik 171927");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("img/cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	
	

	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;

	double characterX=0;
	double characterY=0;
	
	while(!quit) {
		t2 = SDL_GetTicks();
		
	
		

		///////////////animacja postaci/////////////////////////////////
		switch (animate) {
		case RIGHT:
			characterX += SPEED;
			if (characterX >= (getCharacterIndex(board, sizeX*sizeY) % sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_WIDTH - sizeX*IMAGE_SIZE) / 2 )
				animate = NONE;
			break;
		case LEFT:
			characterX -= SPEED;
			if (characterX <= (getCharacterIndex(board, sizeX*sizeY) % sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_WIDTH - sizeX*IMAGE_SIZE) / 2 )
				animate = NONE;
			break;
		case DOWN:
			characterY += SPEED;
			if (characterY >= (getCharacterIndex(board, sizeX*sizeY) / sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_HEIGHT - sizeY*IMAGE_SIZE) / 2 )
				animate = NONE;
			break;
		case UP:
			characterY -= SPEED;
			if (characterY <= (getCharacterIndex(board, sizeX*sizeY) / sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_HEIGHT - sizeY*IMAGE_SIZE) / 2)
				animate = NONE;
			break;
		default:
			characterX = (getCharacterIndex(board, sizeX*sizeY) % sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_WIDTH - sizeX*IMAGE_SIZE) / 2;
			characterY = (getCharacterIndex(board, sizeX*sizeY) / sizeX)*IMAGE_SIZE + 0.5*IMAGE_SIZE + (SCREEN_HEIGHT - sizeY*IMAGE_SIZE) / 2;
			break;
		}
		if (animate != NONE)
			SDL_Delay(10);

		

		
		

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

	
			

	

		SDL_FillRect(screen, NULL, czarny);

		

		fpsTimer += delta;
		if(fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
			};
	
		
		
		
		if (menu == MAIN_MENU)
			showMenu(screen, charset, activeMap,mapsCount,map);
		else if(menu==NEXT_LEVEL_MENU)
			showNextLevelMenu(screen, charset, activeButton, worldTime,activeMap);
		else if(menu==FALSE) {
			drawBoard(screen, board, sizeX, sizeY, map, characterX, characterY, animateBox);
			drawTime(worldTime, charset, screen);
			worldTime += delta;
			if (endOfGame(board, sizeX*sizeY) == TRUE && tempBoard[getCharacterIndex(board, sizeX*sizeY)] != GOAL && animate == NONE) {
				menu = NEXT_LEVEL_MENU;
			}	
		}
		
	

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_RIGHT:
						if (menu==MAIN_MENU && activeMap<mapsCount)
							activeMap++;
						else if (menu==FALSE&&animate==NONE) {
							 moveCharacter(board, RIGHT, sizeX, sizeY, &animateBox,tempBoard);
							animate = RIGHT;
							map.lastDirection = RIGHT;
						}
						else if (menu == NEXT_LEVEL_MENU) {
							if(activeMap<mapsCount)
								activeButton = NEXT_LEVEL_BTN;
						}
						break;
					case SDLK_LEFT:
						if (menu==MAIN_MENU &&activeMap > 1)
							activeMap--;
						else if(menu==FALSE &&animate==NONE) {
							moveCharacter(board, LEFT, sizeX, sizeY, &animateBox,tempBoard);
							animate = LEFT;
							map.lastDirection = LEFT;
						}
						else if (menu == NEXT_LEVEL_MENU) {
							activeButton = MAIN_MENU_BTN;
						}
						break;
					case SDLK_DOWN:
						if (menu == FALSE &&animate == NONE) {
							moveCharacter(board, DOWN, sizeX, sizeY, &animateBox,tempBoard);
							animate = DOWN;
							map.lastDirection = DOWN;
						}
						break;
					case SDLK_UP:
						if (menu == FALSE &&animate == NONE) {
							 moveCharacter(board, UP, sizeX, sizeY,&animateBox,tempBoard);
							animate = UP;
							map.lastDirection = UP;
						}
						break;
					
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_n:
						board=loadBoard(activeMap, board, &sizeX, &sizeY);
						worldTime = 0;
						break;
					case SDLK_SPACE:
						if (menu != FALSE)
							free(tempBoard);
						if (activeButton == MAIN_MENU_BTN && menu == NEXT_LEVEL_MENU) {
							menu = MAIN_MENU;
							break;
						}
						else if (activeButton = NEXT_LEVEL_BTN && menu == NEXT_LEVEL_MENU)
							if(activeMap+1<=mapsCount)
								activeMap++;
						 if (menu!=FALSE) {
							board=loadBoard(activeMap,board,&sizeX,&sizeY);
							tempBoard = createTempBoard(board, sizeX, sizeY);
							menu = FALSE;
						}
						break;
					}
					
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		frames++;
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
