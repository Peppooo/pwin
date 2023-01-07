#pragma once

#define SDL_MAIN_HANDLED
#include "pwin.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <iostream>
#include <string>

#define CENTER_POSITION 0x99fbu81

using namespace std;

namespace pwin {
	class point2D {
	public:
		int x,y;
		point2D() = default;
		point2D(int _x,int _y) {
			x = _x; y = _y;
		};
	};
	namespace colors {
		class rgba {
		public:
			int r = 0;
			int g = 0;
			int b = 0;
			int a = 0;
			rgba() = default;
			rgba(int red,int green,int blue,int alpha) {
				r = red; g = green; b = blue; a = alpha;
			}
		};
		rgba white(255,255,255,SDL_ALPHA_OPAQUE);
		rgba black(0,0,0,SDL_ALPHA_OPAQUE);
	}
	namespace _2d_shapes {
		void quadrilateral_shape(SDL_Renderer* renderer,int x,int y,int width,int height,colors::rgba color) {
			SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
			for(int i = 0; i < width; i++) {
				SDL_RenderDrawLine(renderer,x + i,y,x + i,y + height);
			}
		}
	};
	namespace utils {
		SDL_Texture* textTexture(string message,string fontFile,SDL_Color color,int fontSize,SDL_Renderer* renderer,SDL_Rect* dstRect)
		{
			// Open the font
			TTF_Font* font = TTF_OpenFont(fontFile.c_str(),fontSize);
			if(font == nullptr) {
				std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
				return nullptr;
			}

			// Render the message to an SDL_Surface
			SDL_Surface* surface = TTF_RenderText_Solid(font,message.c_str(),color);
			if(surface == nullptr) {
				TTF_CloseFont(font);
				std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
				return nullptr;
			}

			// Create an SDL_Texture from the surface
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
			if(texture == nullptr) {
				std::cout << "Unable to create texture from rendered text: " << SDL_GetError() << std::endl;
			}

			// Clean up the surface and font
			SDL_FreeSurface(surface);
			TTF_CloseFont(font);

			// Render the texture
			SDL_RenderCopy(renderer,texture,nullptr,dstRect);

			return texture;
		}
		void renderText(SDL_Renderer* renderer,string text,string font,int x,int y,SDL_Color _color,int fontSize) {
			SDL_Texture* texture = textTexture(text,font,_color,fontSize,renderer,new SDL_Rect{0,1,2,3});
			int textureWidth,textureHeight;
			SDL_QueryTexture(texture,nullptr,nullptr,&textureWidth,&textureHeight);
			SDL_Rect renderQuad = {x, y, textureWidth, textureHeight};
			SDL_RenderCopy(renderer,texture,nullptr,&renderQuad);
			SDL_DestroyTexture(texture);
		}
		point2D center(point2D host,point2D obj) {
			return point2D((host.x / 2) - (obj.x / 2),(host.y / 2) - (obj.y / 2));
		}
	}
	int init() {
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		return 0;
	}
	class Button;
	class TextBox;
	class Window {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		colors::rgba _bg;
		int _x,_y;
		int _w,_h;
	public:
		static int count;
		string name = string("window").append(to_string(count));
		vector<Button*> linkedButtons;
		vector<TextBox*> linkedTextBoxes;
		Window(const char* title,int x,int y,int width,int height) {
			window = SDL_CreateWindow(title,x,y,width,height,NULL);
			renderer = SDL_CreateRenderer(window,NULL,NULL);
			_x = x; _y = y;
			_w = width; _h = height;
			count+=1;
		}
		void updatebt();
		void updatetb();
		void setColor(colors::rgba color) {
			SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
		}
		void background(colors::rgba color) {
			_bg = color;
		}
		void text(const char* text) {
			utils::renderText(renderer,string(text),"C:\\Windows\\Fonts\\arial.ttf",200,200,{0,0,0,255},15);
		}
		void update() {
			setColor(_bg);
			SDL_RenderClear(renderer);
			SDL_PumpEvents();
			updatebt();
			updatetb();
			SDL_RenderPresent(renderer);
		}
		SDL_Renderer* raw_rend() {
			return renderer;
		}
	};
	class Button {
	private:
		char* _text;
		bool addedText = false;
		colors::rgba bgColor = colors::rgba(0,0,0,0);
		colors::rgba foreColor = colors::rgba(0,0,0,0);
		int _h,_w;
		int _x,_y;
	public:
		static int count;
		string name = string("button").append(to_string(count));
		bool triggered = false;
		bool clickable = true;
		Window* _window;
		Button(Window *window,int x,int y,int width,int height,colors::rgba bg) {
			window->linkedButtons.push_back(this);
			_h = height; _w = width;
			_x = x; _y = y;
			_window = window;
			bgColor = bg;
			count+=1;
		}
		void update() {
			_2d_shapes::quadrilateral_shape(_window->raw_rend(),_x,_y,_w,_h,bgColor);
			if(addedText) {
				utils::renderText(_window->raw_rend(),string(_text),"C:\\Windows\\Fonts\\arial.ttf",_x,_y,{0,0,0,255},36);
			}
			int xm,ym; Uint32 buttons;
			SDL_PumpEvents();
			buttons = SDL_GetMouseState(&xm,&ym);
			if(xm > _x && xm < _x + _w && ym > _y && ym < _y + _h) {
				if(buttons == SDL_BUTTON_LEFT) {
					if(clickable) {
						triggered = true;
						return;
					}
				}
			}
			triggered = false;
		}
		void addText(const char* text) {
			_text = (char*)text;
			addedText = true;
		}
		void removeText() {
			addedText = false;
		}
	};
	class TextBox {
	private:
		Window* _window;
		int _x,_y,_h,_w;
		colors::rgba _bg;
		static int count;
		bool inT;
	public:
		string name = string("textbox").append(to_string(count));
		string value = "";
		bool enabled = true;
		TextBox(Window* window,int x,int y,int w,int h,colors::rgba bg) {
			_window = window;
			_x = x; _y = y;
			_h = h; _w = w;
			_bg = bg;
			count+=1;
			window->linkedTextBoxes.push_back(this);
		}
		void update() {
			_2d_shapes::quadrilateral_shape(_window->raw_rend(),_x,_y,_w,_h,_bg);
			utils::renderText(_window->raw_rend(),value,"C:\\Windows\\Fonts\\arial.ttf",_x,_y,{0,0,0,255},36);
			int xm,ym; Uint32 buttons; SDL_Event e;
			SDL_PumpEvents(); 
			SDL_PollEvent(&e);
			buttons = SDL_GetMouseState(&xm,&ym);
			if(xm > _x && xm < _x + _w && ym > _y && ym < _y + _h) {
				if(buttons == SDL_BUTTON_LEFT) {
					if(enabled) {
						inT = true;
						return;
					}
				}
			}
			else {
				if(buttons == SDL_BUTTON_LEFT) {
					inT = false;
				}
			}
			if(inT) {
				if(e.type == SDL_KEYDOWN) {
					if(e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
						value.append("\n");
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
						try {
							if((value.size() - 1) > 0) {
								value.resize(value.size() - 1);
								cout << "resizing " << value.size() << endl;
							}
							else if((value.size()) == 1) {
								value = "";
								cout << "setting " << value.size() << endl;
							}
						} catch(exception ex) {}
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
						value.append(" ");
					}
					else {
						value.push_back((char)SDL_GetKeyFromScancode(e.key.keysym.scancode));
					}
					cout << value << endl;
				}
			}
			//cout << inT << endl;
		}
	};
	void Window::updatebt() {
		for(int i = 0; i < this->linkedButtons.size(); i++) {
			this->linkedButtons[i]->update();
		}
	}
	void Window::updatetb() {
		for(int i = 0; i < this->linkedTextBoxes.size(); i++) {
			this->linkedTextBoxes[i]->update();
		}
	}
	// static values
	int Window::count,Button::count,TextBox::count;
}