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
};

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

color_t colors[3] = {
	{ .r = 0xFF, .g = 0x00, .b = 0x00 },
	{ .r = 0x00, .g = 0xFF, .b = 0x00 },
	{ .r = 0x00, .g = 0x00, .b = 0xFF }
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

bool is_top_left(vec2_t* start, vec2_t* end) {
  vec2_t edge = { end->x - start->x, end->y - start->y };
  bool is_top_edge = edge.y == 0 && edge.x > 0;
  bool is_left_edge = edge.y < 0;
  return is_left_edge || is_top_edge;
}

int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p) {
	vec2_t ab = { b->x - a->x, b->y - a->y };
	vec2_t ap = { p->x - a->x, p->y - a->y };
	return ab.x * ap.y - ab.y * ap.x;
}

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2) {
	// Find the bounding box with all candidate pixels
	int x_min = MIN(MIN(v0.x, v1.x), v2.x);
	int y_min = MIN(MIN(v0.y, v1.y), v2.y);
	int x_max = MAX(MAX(v0.x, v1.x), v2.x);
	int y_max = MAX(MAX(v0.y, v1.y), v2.y);
	
	// Compute the area of the entire triangle/parallelogram
	float area = edge_cross(&v0, &v1, &v2);
		
	int bias0 = is_top_left(&v1, &v2) ? 0 : -1;
	int bias1 = is_top_left(&v2, &v0) ? 0 : -1;
	int bias2 = is_top_left(&v0, &v1) ? 0 : -1;
	
	// Loop all candidate pixels inside the bounding box
	for (int y = y_min; y <= y_max; y++) {
		for (int x = x_min; x <= x_max; x++) {
			vec2_t p = {x, y};
			
			int w0 = edge_cross(&v1, &v2, &p) + bias0;
			int w1 = edge_cross(&v2, &v0, &p) + bias1;
			int w2 = edge_cross(&v0, &v1, &p) + bias2;
						
			bool is_inside = w0 >= 0 && w1 >=0 && w2 >=0;
			
			if (is_inside) {
				float alpha = w0 / (float)area;
				float beta = w1 / (float)area;
				float gamma = w2 / (float)area;
				
				int a = 0xFF;
				int r = (alpha) * colors[0].r + (beta) * colors[1].r + (gamma) * colors[2].r;
				int g = (alpha) * colors[0].g + (beta) * colors[1].g + (gamma) * colors[2].g;
				int b = (alpha) * colors[0].b + (beta) * colors[1].b + (gamma) * colors[2].b;
				
				uint32_t interp_color = 0x00000000;
				interp_color = (interp_color | a) << 8;
				interp_color = (interp_color | b) << 8;
				interp_color = (interp_color | g) << 8;
				interp_color = (interp_color | r);
				
				draw_pixel(x, y, interp_color);
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
	
	triangle_fill(v0, v1, v2);
	triangle_fill(v3, v2, v1);
	
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