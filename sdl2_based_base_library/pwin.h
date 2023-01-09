#pragma once

#define SDL_MAIN_HANDLED
#include "pwin.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

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
			SDL_Color sdl_type() {
				return SDL_Color{(Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a};
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
		vector<string> split(const string& str,const string& delimiter) {
			vector<string> parts;
			size_t start = 0,end = 0;
			while(end != string::npos) {
				end = str.find(delimiter,start);
				parts.push_back(str.substr(start,end - start));
				start = end + delimiter.size();
			}
			return parts;
		} 
		string fitReturns(string text,string font,int fontSize,int w,int h) {
			int sW,sH;
			vector<string> words = split(text," ");
			string ret = "";
			string line;
			TTF_Font* fontObject = TTF_OpenFont(font.c_str(),fontSize);
			if(fontObject == NULL) {
				cerr << "Error: Failed to open font " << font << endl;
				return "";
			}
			for(int i = 0; i < words.size(); i++) {
				TTF_SizeText(fontObject,words[i].c_str(),&sW,&sH);
				if(sW > w) {
					size_t pos = 0;
					while(pos < words[i].size()) {
						string chunk = words[i].substr(pos,w);
						TTF_SizeText(fontObject,chunk.c_str(),&sW,&sH);
						while(sW > w) {
							cout << "removed last char " << endl;
							chunk.erase(chunk.size() - 1);
							TTF_SizeText(fontObject,chunk.c_str(),&sW,&sH);
						}
						if(line.size() + chunk.size() > w) {
							cout << "new line" << endl;
							ret.append(line);
							ret.append("\n");
							line = "";
						}
						line.append(chunk);
						line.append("\n");
						pos += chunk.size();
					}
				}
				else {
					if(sW + line.size() > w) {
						cout << "new line" << endl;
						ret.append(line);
						ret.append("\n");
						line = "";
					}
					line.append(words[i]);
					line.append(" ");
				}
			}
			ret.append(line);
			TTF_CloseFont(fontObject);
			return ret;
		}
		int fitFont(string text,string font,int width,int height) {
			int fontSize = 1;
			int prevFontSize;
			TTF_Font* fontObject = TTF_OpenFont(font.c_str(),fontSize);
			if(fontObject == NULL) {
				cerr << "Error: Failed to open font " << font << endl;
				return -1;
			}
			int sW,sH;
			TTF_SizeText(fontObject,text.c_str(),&sW,&sH);
			while(sW <= width && sH <= height) {
				prevFontSize = fontSize;
				fontSize++;
				TTF_SetFontSize(fontObject,fontSize);
				text = fitReturns(text,font,fontSize,width,height);
				TTF_SizeText(fontObject,text.c_str(),&sW,&sH);
			}
			TTF_CloseFont(fontObject);
			return fontSize;
		}

		SDL_Texture* textTexture(string message,string fontFile,SDL_Color fg,int fontSize,SDL_Renderer* renderer)
		{
			// Open the font
			TTF_Font* font = TTF_OpenFont(fontFile.c_str(),fontSize);
			if(font == nullptr) {
				std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
				return nullptr;
			}

			// Render the message to an SDL_Surface
			SDL_Surface* surface = TTF_RenderText_Solid(font,message.c_str(),fg);
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

			return texture;
		}
		void renderText(SDL_Renderer* renderer,string text,string font,int x,int y,SDL_Color fg,int fontSize) {
			SDL_Texture* texture = textTexture(text,font,fg,fontSize,renderer);
			int textureWidth,textureHeight;
			SDL_QueryTexture(texture,nullptr,nullptr,&textureWidth,&textureHeight);
			SDL_Rect renderQuad = {x, y, textureWidth, textureHeight};
			SDL_RenderCopy(renderer,texture,nullptr,&renderQuad);
			SDL_DestroyTexture(texture);
		}
		void renderBoxText(SDL_Renderer* renderer,string text,string font,int x,int y,int w,int h,SDL_Color fg)
		{
			renderText(renderer,text,font,x,y,fg,fitFont(text,font,w,h));
		}
		void renderTextWNewline(SDL_Renderer* renderer,string text,string font,int fontSize,int x,int y,SDL_Color fg) {
			vector<string> lines = split(text,"\n");
			TTF_Font* tfont = TTF_OpenFont(font.c_str(),fontSize);
			int d; TTF_SizeText(tfont,"A",nullptr,&d);
			for(int i = 0; i < lines.size(); i++) {
				//lines[i].erase(std::remove(lines[i].begin(),lines[i].end(),'a'),lines[i].end());
				renderText(renderer,lines[i],font,x,(y + d * i),fg,fontSize);
			}
		}
		void renderTextWReturns(SDL_Renderer* renderer,string text,string font,int fontSize,int x,int y,int w,int h,SDL_Color fg) {
			renderTextWNewline(renderer,fitReturns(text,font,fontSize,w,h),font,fontSize,x,y,fg);
		}
		void fixedTextRenderer(SDL_Renderer* renderer,string text,string font,int baseFontSize,int x,int y,int w,int h,SDL_Color fg) {
			string fixedText = fitReturns(text,font,baseFontSize,w,h);
			renderTextWNewline(renderer,fixedText,font,fitFont(fixedText,font,w,h),x,y,fg);
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
	class keystreak {
	public:
		SDL_Scancode first;
		SDL_Scancode second;
		keystreak() = default;
		keystreak(SDL_Scancode _first,SDL_Scancode _second) {
			first = _first;
			second = _second;
		}
	};
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
		int _fontSize;
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
				utils::renderText(_window->raw_rend(),string(_text),"C:\\Windows\\Fonts\\arial.ttf",_x,_y,{0,0,0,255},_fontSize);
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
		void addText(const char* text, int fontSize) {
			_text = (char*)text;
			addedText = true;
			_fontSize = fontSize;
		}
		void removeText() {
			addedText = false;
		}
	};
	class TextBox { // --- Text Boxes ---
	public:
		string value = "";
	private:
		Window* _window;
		int _x,_y,_h,_w;
		colors::rgba _bg,_fg;
		int _fontsize;
		static int count;
		bool inT = false;
		bool scrollbarU=false;
		int charW,charH;
		string renderedText;
		bool autoReturn = true;
		bool caps = false;
		int cursorIndex = 0;
		keystreak removeWord;
		void renderCursor() {
			TTF_SizeText(TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf",_fontsize),value.c_str(),&charW,&charH);
			_window->setColor(colors::black);
			SDL_RenderDrawLine(_window->raw_rend(),_x + charW,_y,_x + charW,_y+charH);
			//cout << "printing cursor at with x: " << _x << " char x size: " << charW << " cursor index: " << cursorIndex << endl;
		}
	public:
		string name = string("textbox").append(to_string(count));
		bool enabled = true;
		TextBox(Window* window,int x,int y,int w,int h,colors::rgba bg, colors::rgba fg,int fontsize) {
			_window = window;
			_x = x; _y = y;
			_h = h; _w = w;
			_bg = bg;
			count += 1;
			_fontsize = fontsize;
				
			window->linkedTextBoxes.push_back(this);
		}
		void update() {
			_2d_shapes::quadrilateral_shape(_window->raw_rend(),_x,_y,_w,_h,_bg);
			utils::renderText(_window->raw_rend(),value,"C:\\Windows\\Fonts\\arial.ttf",_x,_y,_fg.sdl_type(),_fontsize);
			if(inT) {
				renderCursor();
			}
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
				// draw cursor
				_window->setColor(colors::black);

				//SDL_RenderDrawLine(_window->raw_rend(),1,1,2,2);
				if(e.type == SDL_KEYDOWN) {
					if(e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
						value.at(cursorIndex) = (char)'\n';
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
						if(!(cursorIndex <= 0)) {
							cursorIndex--;
						}
						try {
							if((value.size() - 1) > 0) {
								value.resize(value.size() - 1);
								//cout << "resizing " << value.size() << endl;
							}
							else if((value.size()) == 1) {
								value = "";
								//cout << "setting " << value.size() << endl;
							}
						} catch(exception ex) {} 
						
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
						value.append(" ");
						cursorIndex++;
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						inT = false;
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_CAPSLOCK) {
						caps = !caps;
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_LSHIFT || e.key.keysym.scancode == SDL_SCANCODE_RSHIFT) {
						caps = !caps;
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						if(!(cursorIndex<=0)) {
							cursorIndex--;
						}
					}
					else if(e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						if(!(cursorIndex <= value.size())) {
							cursorIndex++;
						}
					}
					else {
						value.resize(cursorIndex+1);
						if(caps) {
							cout << "cursor index: " << cursorIndex << endl;
							value.at(cursorIndex) = ((char)(SDL_GetKeyFromScancode(e.key.keysym.scancode)-32));
						} else {
							value.at(cursorIndex) = ((char)SDL_GetKeyFromScancode(e.key.keysym.scancode));
						}
						cursorIndex++;
					}
					cout << "textbox value: " << value << endl;
				}
				else if(e.type == SDL_KEYUP) {
					if(e.key.keysym.scancode == SDL_SCANCODE_LSHIFT || e.key.keysym.scancode == SDL_SCANCODE_RSHIFT) {
						caps = !caps;
					}
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