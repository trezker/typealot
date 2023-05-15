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

#define MAX_WORDS 80000
ALLEGRO_USTR* dictionary[MAX_WORDS];
int words = 0;

void load_dictionary(const char* filename) {
	printf("Loading dictionary\n");
	FILE* f = fopen(filename, "r");
	char w[100];
	int removed = 0;
	while(fgets(w, 100, f)) {
		if(words == MAX_WORDS) {
			printf("Dictionary file has more words than is supported\n");
			break;
		}
		ALLEGRO_USTR *ustr = al_ustr_new(w);
		al_ustr_rtrim_ws(ustr);
		if(-1 != al_ustr_find_cstr(ustr, 0, "'s")) {
			++removed;
			continue;
		}
		dictionary[words++] = ustr;
		//printf("%s", w);
	}
	fclose(f);
	printf("Dictionary loaded. Removed %i, kept %i\n", removed, words);
}

void add_word(ALLEGRO_USTR* str) {
	al_ustr_append(str, dictionary[rand()%words]);
	al_ustr_append_cstr(str, " ");
}

void initial_words(ALLEGRO_USTR* str) {
	for(int i = 0; i < 5; ++i) {
		add_word(str);
	}
}

int main() {
	srand(time(NULL));
	load_dictionary("data/words.txt");

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
	int width = 800;
	int height = 200;
	display = al_create_display(width, height);
	if (!display) {
		abort_dialog("Error creating display\n");
	}

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	ALLEGRO_FONT *font = al_load_ttf_font("data/DejaVuSans.ttf", 40, 0);
	ALLEGRO_FONT *fontsmall = al_load_ttf_font("data/DejaVuSans.ttf", 20, 0);

	ALLEGRO_COLOR black = al_map_rgb_f(0, 0, 0);
	ALLEGRO_COLOR grey = al_map_rgb_f(0.5, 0.5, 0.5);
	ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

	ALLEGRO_USTR *next = al_ustr_new("");
	ALLEGRO_USTR *prev = al_ustr_new("");

	initial_words(next);

	double start_time = -1;
	double elapsed = 0;
	double besttime = 0;
	int wordcount = 0;
	int bestwords = 0;
	int typed = 0;
	int wpm = 0;
	int bestwpm = 0;

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
						++typed;
						if(start_time<0) {
							start_time = al_get_time();
							wordcount = 0;
							elapsed = 0;
						}
						al_ustr_append_chr(prev, event.keyboard.unichar);
						al_ustr_remove_chr(next, 0);
						//printf("%i\n", event.keyboard.unichar);
						if(event.keyboard.unichar == 32) {
							add_word(next);
							++wordcount;
						}
					}
					else {
						if(wpm > bestwpm) {
							bestwpm = wpm;
						}
						if(elapsed > besttime) {
							besttime = elapsed;
						}
						if(wordcount > bestwords) {
							bestwords = wordcount;
						}
						al_ustr_assign_cstr(prev, "");
						al_ustr_assign_cstr(next, "");
						initial_words(next);
						start_time = -1;
						typed = 0;
					}
					break;
			}
		}

		al_clear_to_color(black);

		if(start_time > 0) {
			elapsed = al_get_time()-start_time;
			wpm = (typed/5)/(elapsed/60.f);
		}
		al_draw_textf(fontsmall, white, 0, 0, ALLEGRO_ALIGN_LEFT, "WPM: %i", wpm);
		al_draw_textf(fontsmall, white, 0, 40, ALLEGRO_ALIGN_LEFT, "Best WPM: %i", bestwpm);

		al_draw_textf(fontsmall, white, width/2, 0, ALLEGRO_ALIGN_CENTRE, "%.1f", elapsed);
		al_draw_textf(fontsmall, white, width/2, 40, ALLEGRO_ALIGN_CENTRE, "Best time: %.1f", besttime);

		al_draw_textf(fontsmall, white, width, 0, ALLEGRO_ALIGN_RIGHT, "Words: %i", wordcount);
		al_draw_textf(fontsmall, white, width, 40, ALLEGRO_ALIGN_RIGHT, "Best words %i", bestwords);

		al_draw_ustr(font, grey, width/2, height/2, ALLEGRO_ALIGN_RIGHT, prev);
		al_draw_ustr(font, white, width/2, height/2, ALLEGRO_ALIGN_LEFT, next);

		al_draw_line(width/2, height/2, width/2, height/2+40, white, 2);

		al_flip_display();

		al_rest(0.001);
	}
	
	al_ustr_free(next);
	al_destroy_font(font);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	return EXIT_SUCCESS;
}