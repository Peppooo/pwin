#define SDL_MAIN_HANDLED
#include "pwin.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <stdexcept>

#include <string>

using namespace std;

SDL_Renderer* renderer;
SDL_Window* window;

namespace pwin {
	namespace colors {
		class rgba {
		public:
			int r = 0;
			int g = 0;
			int b = 0;
			int a = 0;
			void set(int red, int green, int blue, int alpha) {
				r = red; g = green; b = blue; a = alpha;
			}
		};
		rgba white;
		rgba black; 
	}
	namespace _2d_shapes {
		void quadrilateral_shape(int xPOS, int yPOS, int base, int height, colors::rgba color) {
			SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
			for(int i = 0; i < base; i++) {
				SDL_RenderDrawLine(renderer,xPOS + i,yPOS - height,xPOS + i,yPOS);
			}
		}
	};
	int init() {
		colors::white.set(255,255,255,SDL_ALPHA_OPAQUE);
		colors::black.set(0,0,0,SDL_ALPHA_OPAQUE);
		return SDL_Init(SDL_INIT_EVERYTHING);
	}
	int create_window(int x,int y) {
		return SDL_CreateWindowAndRenderer(x,y,NULL,&window,&renderer);
	}
	void update() {
		SDL_RenderPresent(renderer);
	}
	void set_color(colors::rgba color) {
		SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
	}
	void draw_button(int xPOS, int yPOS, int base, int height, colors::rgba color) {
		_2d_shapes::quadrilateral_shape(xPOS,yPOS,base,height, color);
	}
};