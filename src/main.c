#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

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

char* dictionary[] = {
	"it",
	"eats",
	"pigs",
	"and",
	"cows"
};

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
	al_init_font_addon();
	al_init_ttf_addon();

	al_set_new_display_flags(ALLEGRO_OPENGL);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
	int width = 640;
	int height = 120;
	display = al_create_display(width, height);
	if (!display) {
		abort_dialog("Error creating display\n");
	}

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	ALLEGRO_FONT *font = al_load_ttf_font("data/DejaVuSans.ttf", 42, 0);

	ALLEGRO_COLOR black = al_map_rgb_f(0, 0, 0);
	ALLEGRO_COLOR grey = al_map_rgb_f(0.5, 0.5, 0.5);
	ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

	ALLEGRO_USTR *next = al_ustr_new("");
	ALLEGRO_USTR *prev = al_ustr_new("");

	for(int i = 0; i < 3; ++i) {
		al_ustr_append_cstr(next, dictionary[i]);
		al_ustr_append_cstr(next, " ");
	}

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
				case ALLEGRO_EVENT_KEY_CHAR:
					if(0 == al_ustr_find_chr(next, 0, event.keyboard.unichar)) {
						al_ustr_append_chr(prev, event.keyboard.unichar);
						al_ustr_remove_chr(next, 0);
					}
					break;
			}
		}

		al_clear_to_color(black);

		//al_draw_text(font, white, width/2, height/2, ALLEGRO_ALIGN_CENTRE, "Hello world!");
		al_draw_ustr(font, grey, width/2, height/2, ALLEGRO_ALIGN_RIGHT, prev);
		al_draw_ustr(font, white, width/2, height/2, ALLEGRO_ALIGN_LEFT, next);

		al_flip_display();

		al_rest(0.001);
	}
	
	al_ustr_free(next);
	al_destroy_font(font);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	return EXIT_SUCCESS;
}