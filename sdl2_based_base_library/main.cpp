#include "pwin.h"

using namespace std;

int main() {
	pwin::init();
	pwin::create_window(1920, 1080);
	pwin::background(pwin::colors::white);
	bool t = false;
	while(1) {
		pwin::draw_button(200,200,50,50,pwin::colors::black, &t);
		cout << t << endl;
		if(t == true) {
			break;
		}
		pwin::update();
	}
	
	return 0;
}