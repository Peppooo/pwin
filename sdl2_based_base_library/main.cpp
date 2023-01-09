#include "pwin.h"

using namespace std;
using namespace pwin;

int main() {
	init();
	colors::rgba grey_bg = colors::rgba(25,25,25,1);
	point2D screenCenter = utils::center(point2D(2560,2440),point2D(1920,1080));
	point2D tbCenter = utils::center(point2D(1920,1080),point2D(500,100));

	Window window("window",screenCenter.x,screenCenter.y,1920,1080);
	TextBox textbox(&window,tbCenter.x,tbCenter.y,500,100,colors::white,colors::black,20);
	window.background(colors::rgba(grey_bg));
	while(true) {
		window.update();
	};
}