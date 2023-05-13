#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

void abort_dialog(char const *format, ...) {
	char str[1024];
	va_list args;
	ALLEGRO_DISPLAY *display;

	va_start(args, format);
	vsnprintf(str, sizeof str, format, args);
	va_end(args);

	if (al_init_native_dialog_addon()) {
		display = al_is_system_installed() ? al_get_current_display() : NULL;
		al_show_native_message_box(display, "Error", "Cannot run", str, NULL, 0);
	}
	else {
		fprintf(stderr, "%s", str);
	}
	exit(1);
}

int main() {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_EVENT event;

	if (!al_init()) {
		abort_dialog("Could not init Allegro.\n");
	}

	al_init_primitives_addon();
	al_install_mouse();
	al_install_keyboard();

	al_set_new_display_flags(ALLEGRO_OPENGL);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
	display = al_create_display(640, 480);
	if (!display) {
		abort_dialog("Error creating display\n");
	}

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

//	ALLEGRO_COLOR black = al_map_rgb_f(0, 0, 0);
//	ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

	int done = 0;
	while(!done) {
		while(al_get_next_event(queue, &event)) {
			switch(event.type) {
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					done = 1;
					break;
				case ALLEGRO_EVENT_KEY_DOWN:
					if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						done = 1;
					}
					break;
			}
		}
	}

	return EXIT_SUCCESS;
}