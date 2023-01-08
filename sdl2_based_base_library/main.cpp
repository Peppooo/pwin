#include "pwin.h"

using namespace std;
using namespace pwin;

int main() {
	init();
	colors::rgba grey_bg = colors::rgba(25,25,25,1);
	Window window("window", 2560/2-(1920/2),1440/2-((1080 / 2)),1920,1080);
	TextBox textbox(&window,1920/2-100/2,1080/2-100/2,100,100,colors::white,colors::black,20);
	window.background(colors::rgba(grey_bg));
	//cout << utils::fitReturns("test test test","C:\\Windows\\Fonts\\arial.ttf",30,50,50) << endl;
	while(true) {
		window.update();
	};
}