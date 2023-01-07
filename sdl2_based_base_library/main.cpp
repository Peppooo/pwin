#include "pwin.h"

using namespace std;
using namespace pwin;

int main() {
	init();
	colors::rgba grey_bg = colors::rgba(25,25,25,1);
	Window window("window", 2560/2-(1920/2),1440/2-((1080 / 2)),1920,1080);
	TextBox textbox(&window,1920/2-100/2,1080/2-100/2,100,100,colors::white);
	window.background(colors::rgba(grey_bg));
	while(true) {
		//cout << textbox.value << endl;
		window.update();
	};
}