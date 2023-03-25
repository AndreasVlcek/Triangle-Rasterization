#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vec2.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

bool is_running = false;

vec2_t vertices[4] = {
	{ .x = 40, .y = 40 },
	{ .x = 80, .y = 40 },
	{ .x = 40, .y = 80 },
	{ .x = 90, .y = 90 },
	{ .x = 75, .y = 20 },
};

void process_input(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				is_running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					is_running = false;
				break;
		}
	}
}

int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p) {
	vec2_t ab = { b->x - a->x, b->y - a->y };
	vec2_t ap = { p->x - a->x, p->y - a->y };
	return ab.x * ap.y - ab.y * ap.x;
}

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, uint32_t color) {
	// Find the bounding box with all candidate pixels
	int x_min = MIN(MIN(v0.x, v1.x), v2.x);
	int y_min = MIN(MIN(v0.y, v1.y), v2.y);
	int x_max = MAX(MAX(v0.x, v1.x), v2.x);
	int y_max = MAX(MAX(v0.y, v1.y), v2.y);
	
	// Loop all candidate pixels inside the bounding box
	for (int y = y_min; y <= y_max; y++) {
		for (int x = x_min; x <= x_max; x++) {
			vec2_t p = {x, y};
			
			int w0 = edge_cross(&v1, &v2, &p);
			int w1 = edge_cross(&v2, &v0, &p);
			int w2 = edge_cross(&v0, &v1, &p);
			
			bool is_inside = w0 >= 0 && w1 >=0 && w2 >=0;
			
			if (is_inside) {
				draw_pixel(x, y, 0xFF00FF00);
			}
		}
	}
}

void render(void) {
	clear_framebuffer(0xFF000000);
	
	vec2_t v0 = vertices[0];
	vec2_t v1 = vertices[1];
	vec2_t v2 = vertices[2];
	vec2_t v3 = vertices[3];
	vec2_t v3 = vertices[4];
	
	triangle_fill(v0, v1, v2, 0xFF00FF00);
	triangle_fill(v3, v2, v1, 0xFFA74De3);
	
	render_framebuffer();
}


int main(void) {

	is_running = create_window();
	
	while (is_running) {
		fix_framerate();
		process_input();
		render();
	}
	
	destroy_window();
	
	return EXIT_SUCCESS;
}