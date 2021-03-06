////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// invaderer example: simple game with sprites and sounds
//
// Level: 1
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Simple game mechanics
//   Texture loaded from GIF file
//   Audio
//

namespace octet {
	class sprite {
		// where is our sprite (overkill for a 2D game!)
		mat4t modelToWorld;

		// half the width of the sprite
		float halfWidth;

		// half the height of the sprite
		float halfHeight;

		// what texture is on our sprite
		int texture;

		// true if this sprite is enabled.
		bool enabled;

	public:
		sprite() {
			texture = 0;
			enabled = true;
		}

		void init(int _texture, float x, float y, float w, float h) {
			modelToWorld.loadIdentity();
			modelToWorld.translate(x, y, 0);
			halfWidth = w * 0.5f;
			halfHeight = h * 0.5f;
			texture = _texture;
			enabled = true;
		}

		void render(texture_shader &shader, mat4t &cameraToWorld) {
			// invisible sprite... used for gameplay.
			if (!texture) return;

			// build a projection matrix: model -> world -> camera -> projection
			// the projection space is the cube -1 <= x/w, y/w, z/w <= 1
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			// set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			// use "old skool" rendering
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			shader.render(modelToProjection, 0);

			// this is an array of the positions of the corners of the sprite in 3D
			// a straight "float" here means this array is being generated here at runtime.
			float vertices[] = {
			  -halfWidth, -halfHeight, 0,
			   halfWidth, -halfHeight, 0,
			   halfWidth,  halfHeight, 0,
			  -halfWidth,  halfHeight, 0,
			};

			// attribute_pos (=0) is position of each corner
			// each corner has 3 floats (x, y, z)
			// there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
			glEnableVertexAttribArray(attribute_pos);

			// this is an array of the positions of the corners of the texture in 2D
			static const float uvs[] = {
			   0,  0,
			   1,  0,
			   1,  1,
			   0,  1,
			};

			// attribute_uv is position in the texture of each corner
			// each corner (vertex) has 2 floats (x, y)
			// there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
			glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvs);
			glEnableVertexAttribArray(attribute_uv);

			// finally, draw the sprite (4 vertices)
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		// move the object
		void translate(float x, float y) {
			modelToWorld.translate(x, y, 0);
		}

		void rotate(float x) {
			modelToWorld.rotate(x, 0, 0, 1);
		}

		// position the object relative to another.
		void set_relative(sprite &rhs, float x, float y) {
			modelToWorld = rhs.modelToWorld;
			modelToWorld.translate(x, y, 0);
		}

		// return true if this sprite collides with another.
		// note the "const"s which say we do not modify either sprite
		bool collides_with(const sprite &rhs) const {
			float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
			float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

			// both distances have to be under the sum of the halfwidths
			// for a collision
			return
				(fabsf(dx) < halfWidth + rhs.halfWidth) &&
				(fabsf(dy) < halfHeight + rhs.halfHeight)
				;
		}

		bool is_above(const sprite &rhs, float margin) const {
			float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];

			return
				(fabsf(dx) < halfWidth + margin)
				;
		}

		bool &is_enabled() {
			return enabled;
		}
	};

	class invaderers_app : public octet::app {
		// Matrix to transform points in our camera space to the world.
		// This lets us move our camera
		mat4t cameraToWorld;

		// shader to draw a textured triangle
		texture_shader texture_shader_;

		enum {
			num_music_sound_source = 1,
			num_sound_sources = 8,
			num_rows = 4,
			num_cols = 10,
			num_missiles = 40,
			num_bombs = 2,
			num_S_powerups = 6,
			num_borders = 4,
			num_invaderers = num_rows * num_cols,
			camera_shake = 0,

			// sprite definitions
			ship_sprite = 0,
			game_over_sprite,
			shipC_sprite,

			first_S_sprite,
			last_S_sprite = first_S_sprite + num_S_powerups - 1,

			first_invaderer_sprite,
			last_invaderer_sprite = first_invaderer_sprite + num_invaderers - 1,

			first_missile_sprite,
			last_missile_sprite = first_missile_sprite + num_missiles - 1,

			first_bomb_sprite,
			last_bomb_sprite = first_bomb_sprite + num_bombs - 1,

			first_border_sprite,
			last_border_sprite = first_border_sprite + num_borders - 1,

			num_sprites,

		};

		// timers for missiles and bombs
		int missiles_disabled;
		int bombs_disabled;

		int aInt;
		int bInt;

		// random direction for bullets...
		bool canChangeRandInt = false;


		// accounting for bad guys
		int live_invaderers;
		int num_lives;
		int chargeTimer = 0;

		// game state
		bool game_over;
		int score;

		// speed of enemy
		float invader_velocity;

		// sounds
		ALuint whoosh;
		ALuint bang;

		ALuint powerup;

		ALuint enemy_hit;
		ALuint enemy_shoot;

		ALuint player_shoot;
		ALuint player_hurt;
		ALuint player_dead;

		//
		ALuint music;
		//
		unsigned cur_source;
		ALuint sources[num_sound_sources];
		unsigned cur_music_source;
		ALuint sources2[num_music_sound_source];
		//

		// big array of sprites
		sprite sprites[num_sprites];

		// random number generator
		class random randomizer;

		// a texture for our text
		GLuint font_texture;

		// information for our text
		bitmap_font font;

		ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }
		ALuint get_music_source() { return sources2[0]; }

		// called when we hit an enemy
		void on_hit_invaderer() {
			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, enemy_hit);
			alSourcePlay(source);

			live_invaderers--;
			score++;

			invader_velocity *= 1.05f;

			if (live_invaderers == 0)
			{
				create_Invaders();
				//rather clever, this -
				//initial velocity is 0.01
				//when the game resets, its going to be 40 / 0.0005f, which = 0.02
				//then at 80, 0.04
				//and so on.
				invader_velocity = 0.01f + (score * 0.00008f);
				live_invaderers = num_invaderers;
			}
		}

		// called when we are hit
		void on_hit_ship() {
			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, player_hurt);
			alSourcePlay(source);

			if (--num_lives == 0) {
				game_over = true;
				sprites[game_over_sprite].translate(-20, 0);
				
			}
		}

		void on_S_ship() {
			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, powerup);
			alSourcePlay(source);

			
		}

		// use the keyboard to move the ship
		void move_ship() {
			const float ship_speed = 0.05f;
			// left and right arrows
			if (is_key_down(key_left)) {
				sprites[ship_sprite].translate(-ship_speed, 0);
				if (sprites[ship_sprite].collides_with(sprites[first_border_sprite + 2])) {

					//completely negates previous movement code... cheeky!
					sprites[ship_sprite].translate(+ship_speed, 0);
				}
			}
			else if (is_key_down(key_right)) {
				sprites[ship_sprite].translate(+ship_speed, 0);
				if (sprites[ship_sprite].collides_with(sprites[first_border_sprite + 3])) {
					sprites[ship_sprite].translate(-ship_speed, 0);
				}
			}
			else if (is_key_down(key_backspace)) {
				sprites[ship_sprite].rotate(5);
			}
		}

		// fire button (space)
		void fire_missiles()
		{
			if (missiles_disabled)
			{
				--missiles_disabled;
			}
			else if (is_key_down(' '))
			{
				chargeTimer++;
			}
			if (is_key_going_up(' '))
			{
				// find a missile
				
					for (int i = (chargeTimer/30) + 1; i != num_missiles; ++i)
					{
						if (!sprites[first_missile_sprite + i].is_enabled())
						{
							printf("%i\n", i);
							sprites[first_missile_sprite + i].set_relative(sprites[ship_sprite], 0, 0.5f);
							sprites[first_missile_sprite + i].is_enabled() = true;
							//
							//
							//
							//
							//TIMER INBETWEEN PLAYER SHOTS!!!
							//missiles_disabled = 1;
							ALuint source = get_sound_source();
							alSourcei(source, AL_BUFFER, player_shoot);
							alSourcePlay(source);
							break;
						}
					}
					chargeTimer = 0;

				
			}
			
			if (camera_shake == 0)
			{
				//cameraToWorld.translate(0.001f, 0.001f, 0.001f);




				//cameraToWorld.set_relative(0, 0, 0);
				//cameraToWorld.translate(cameraToWorld, cameraToWorld.y(), 0);
				//cameraToWorld.x() = 1;
			}
			
		}

		// pick an invader and fire a bomb
		void fire_bombs() {
			if (bombs_disabled) {
				--bombs_disabled;
			}
			else {
				// find an invaderer
				sprite &ship = sprites[ship_sprite];
				float xVel = invader_velocity;
				for (int j = randomizer.get(0, num_invaderers); j < num_invaderers; ++j) {
					sprite &invaderer = sprites[first_invaderer_sprite + j];
					if (invaderer.is_enabled() && invaderer.is_above(ship, 0.3f)) {
						// find a bomb
						for (int i = 0; i != num_bombs; ++i) {
							if (!sprites[first_bomb_sprite + i].is_enabled()) {
								sprites[first_bomb_sprite + i].set_relative(invaderer, 0, -0.25f);
								sprites[first_bomb_sprite + i].is_enabled() = true;
								//sprites[first_bomb_sprite + i].bomb_Xspeed = invader_velocity;
								bombs_disabled = 30;
								ALuint source = get_sound_source();
								alSourcei(source, AL_BUFFER, enemy_shoot);
								alSourcePlay(source);
								return;
							}
						}
						return;
					}
				}
			}
		}

		// animate the missiles
		void move_missiles() {
			for (int i = 0; i != num_missiles; ++i) {
				sprite &missile = sprites[first_missile_sprite + i];
				if (missile.is_enabled()) {

					
					float missile_speed = 0.26f;
					//float missile_speed = ((rand() % 4) + 1) / 4;

					//
					missile.translate(0, missile_speed);
					//

					for (int j = 0; j != num_invaderers; ++j) {
						sprite &invaderer = sprites[first_invaderer_sprite + j];
						if (invaderer.is_enabled() && missile.collides_with(invaderer)) {
							invaderer.is_enabled() = false;

							//
							//
							//

							int randInt = rand() % 4;
							if (randInt == 3)
							{
								printf("drop a ting!");
								for (int i = 0; i != num_S_powerups; ++i)
								{
									if (!sprites[first_S_sprite + i].is_enabled())
									{
										sprites[first_S_sprite + i].set_relative(invaderer, 0, -0.25f);
										sprites[first_S_sprite + i].is_enabled() = true;
										//TIMER INBETWEEN PLAYER SHOTS!!!
										// missiles_disabled = 1;
										ALuint source = get_sound_source();
										alSourcei(source, AL_BUFFER, bang);
										alSourcePlay(source);
										break;
									}
								}
							}
							invaderer.translate(20, 0);
							missile.is_enabled() = false;
							missile.translate(20, 0);
							on_hit_invaderer();

							goto next_missile;
						}
					}
					if (missile.collides_with(sprites[first_border_sprite + 1])) {
						missile.is_enabled() = false;
						missile.translate(20, 0);
					}
				}
			next_missile:;
			}
		}

		//shiggy
		// animate the bombs
		void move_bombs() {
			const float bomb_speed = 0.2f;
			for (int i = 0; i != num_bombs; ++i) {
				sprite &bomb = sprites[first_bomb_sprite + i];
				if (bomb.is_enabled()) {
					bomb.translate(0, -bomb_speed);
					if (bomb.collides_with(sprites[ship_sprite])) {
						bomb.is_enabled() = false;
						bomb.translate(20, 0);
						bombs_disabled = 50;
						on_hit_ship();
						goto next_bomb;
					}
					if (bomb.collides_with(sprites[first_border_sprite + 0])) {
						bomb.is_enabled() = false;
						bomb.translate(20, 0);
					}
				}
			next_bomb:;
			}
		}

		void move_S() {
			const float bomb_speed = 0.1f;
			for (int i = 0; i != num_S_powerups; ++i) {
				sprite &S_powerup = sprites[first_S_sprite + i];

				
				if (S_powerup.is_enabled()) {

					S_powerup.translate(sin(0), -bomb_speed);

					if (S_powerup.collides_with(sprites[ship_sprite])) {
						S_powerup.is_enabled() = false;
						S_powerup.translate(20, 0);
						//bombs_disabled = 50;
						//for now, give extra ship...
						num_lives++;

						on_S_ship();
						goto next_S;
					}
					if (S_powerup.collides_with(sprites[first_border_sprite + 0])) {
						S_powerup.is_enabled() = false;
						S_powerup.translate(20, 0);
					}
				}
			next_S:;
			}
		}

		// move the array of enemies
		void move_invaders(float dx, float dy) {
			for (int j = 0; j != num_invaderers; ++j) {
				sprite &invaderer = sprites[first_invaderer_sprite + j];
				if (invaderer.is_enabled()) {
					invaderer.translate(dx, dy);
				}
			}
		}

		// check if any invaders hit the sides.
		bool invaders_collide(sprite &border) {
			for (int j = 0; j != num_invaderers; ++j) {
				sprite &invaderer = sprites[first_invaderer_sprite + j];
				if (invaderer.is_enabled() && invaderer.collides_with(border)) {
					return true;
				}
			}
			return false;
		}

		void create_Invaders()
		{
			GLuint invaderer = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/invaderer2.gif");
			//GLuint invaderer2 = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/invaderer3.gif");

			for (int j = 0; j != num_rows; ++j) {
				for (int i = 0; i != num_cols; ++i) {
					assert(first_invaderer_sprite + i + j*num_cols <= last_invaderer_sprite);
					sprites[first_invaderer_sprite + i + j*num_cols].init(
						invaderer, ((float)i - num_cols * 0.5f) * 0.5f, 2.50f - ((float)j * 0.5f), 0.25f, 0.25f);
					int v1 = rand() % 100;
					printf("eh" + v1);
				}
			}
		}


		void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
			mat4t modelToWorld;
			modelToWorld.loadIdentity();
			modelToWorld.translate(x, y, 0);
			modelToWorld.scale(scale, scale, 1);
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			/*mat4t tmp;
			glLoadIdentity();
			glTranslatef(x, y, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
			glScalef(scale, scale, 1);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

			enum { max_quads = 32 };
			bitmap_font::vertex vertices[max_quads * 4];
			uint32_t indices[max_quads * 6];
			aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

			unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, font_texture);

			shader.render(modelToProjection, 0);

			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x);
			glEnableVertexAttribArray(attribute_pos);
			glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u);
			glEnableVertexAttribArray(attribute_uv);

			glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
		}

	public:

		// this is called when we construct the class
		invaderers_app(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {
		}

		// this is called once OpenGL is initialized
		void app_init() {



			// set up the shader
			texture_shader_.init();

			// set up the matrices with a camera 5 units from the origin
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, 0, 3);

			font_texture = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");

			GLuint shipC = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/ship.gif");
			sprites[shipC_sprite].init(shipC, 0, 0, 0.1f, 0.1f);

			GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/ship.gif");
			sprites[ship_sprite].init(ship, 0, -2.75f, 0.25f, 0.25f);
			

			GLuint GameOver = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/GameOver.gif");
			sprites[game_over_sprite].init(GameOver, 20, 0, 3, 1.5f);

			GLuint invaderer = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/invaderer2.gif");
			//GLuint invaderer2 = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/invaderer3.gif");


			create_Invaders();


			// set the border to white for clarity
			GLuint white = resource_dict::get_texture_handle(GL_RGB, "#ffffff");
			sprites[first_border_sprite + 0].init(white, 0, -3, 6, 0.2f);
			sprites[first_border_sprite + 1].init(white, 0, 3, 6, 0.2f);
			sprites[first_border_sprite + 2].init(white, -3, 0, 0.2f, 6);
			sprites[first_border_sprite + 3].init(white, 3, 0, 0.2f, 6);

			// use the S texture
			GLuint S_powerup = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/S.gif");
			for (int i = 0; i != num_S_powerups; i++)
			{
				//create S' off screen
				sprites[first_S_sprite + i].init(S_powerup, 20, 0, 0.25f,0.25f);
				sprites[first_S_sprite + 1].is_enabled() = false;

			}

			// use the missile texture
			GLuint missile = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile.gif");
			for (int i = 0; i != num_missiles; ++i) {
				// create missiles off-screen
				sprites[first_missile_sprite + i].init(missile, 20, 0, 0.0625f, 0.25f);
				sprites[first_missile_sprite + i].is_enabled() = false;
			}

			// use the bomb texture
			GLuint bomb = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/bomb.gif");
			for (int i = 0; i != num_bombs; ++i) {
				// create bombs off-screen
				sprites[first_bomb_sprite + i].init(bomb, 20, 0, 0.0625f, 0.25f);
				sprites[first_bomb_sprite + i].is_enabled() = false;
			}

			// sounds
			whoosh = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/whoosh.wav");
			bang = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");
			music = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/music.wav");

			powerup = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/powerup.wav");


			player_shoot = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/player_shoot.wav");
			player_hurt = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/player_hurt.wav");
			player_dead = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/player_dead.wav");

			enemy_shoot = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/enemy_shoot.wav");
			enemy_hit = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/enemy_hit.wav");



			cur_source = 0;
			alGenSources(num_sound_sources, sources);
			alGenSources(num_music_sound_source, sources2);

			// sundry counters and game state.
			missiles_disabled = 0;
			bombs_disabled = 50;		//5 seconds before enemies can shoot
			invader_velocity = 0.01f;
			live_invaderers = num_invaderers;
			num_lives = 3;
			game_over = false;
			score = 0;


			ALuint source2 = get_music_source();
			//ASK HOW TO MAKE IT LOOP!!!
			alSourcei(source2, AL_BUFFER, music);
			alSourcePlay(source2);



		}

		// called every frame to move things
		void simulate() {
			if (game_over) {
				return;
			}

			move_ship();

			fire_missiles();

			fire_bombs();

			move_missiles();

			move_bombs();

			move_S();

			move_invaders(invader_velocity, 0);

			sprite &border = sprites[first_border_sprite + (invader_velocity < 0 ? 2 : 3)];
			if (invaders_collide(border)) {
				invader_velocity = -invader_velocity;
				move_invaders(invader_velocity, -0.1f);
			}
		}

		// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			simulate();

			// set a viewport - includes whole window area
			glViewport(x, y, w, h);

			// clear the background to black
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
			glDisable(GL_DEPTH_TEST);

			// allow alpha blend (transparency when alpha channel is 0)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// draw all the sprites
			for (int i = 0; i != num_sprites; ++i) {
				sprites[i].render(texture_shader_, cameraToWorld);
			}

			char score_text[32];
			sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
			draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, score_text);

			// move the listener with the camera
			vec4 &cpos = cameraToWorld.w();
			alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
		}
	};
}
