////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
	/// Scene containing a box with octet.
	class example_shapes : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;

		int lPlank_xInt = -28;
		int lPlank_yInt = 0;

		int rPlank_xInt = 28;
		int rPlank_yInt = 6;


		int numPlanks = 20;
		mesh_instance *m_bridge[20];
		btRigidBody *rb_bridge[20];


	public:

		mat4t mat;
		material *blue = new material(vec4(0, 0, 1, 1));

		example_shapes(int argc, char **argv) : app(argc, argv) {
		}

		~example_shapes() {
		}


		/// this is called once OpenGL is initialized
		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, numPlanks + (numPlanks*0.5f)));

			material *red = new material(vec4(1, 0, 0, 1));
			material *green = new material(vec4(0, 1, 0, 1));
			material *purple = new material(vec4(1, 0, 1, 1));
			material *black = new material(vec4(0,0,0, 1));

			mat.loadIdentity();
		    mat.translate(0, 0, -10);
			app_scene->add_shape(mat, new mesh_box(vec3(200, 200, 0.5f)), black, false);



			//create first green plank

			mat.loadIdentity();
			mat.translate(lPlank_xInt, lPlank_yInt, 0);
			m_bridge[0] = app_scene->add_shape(mat, new mesh_box(vec3(0.1f, 0.1f, 10)), green, false);
			rb_bridge[0] = m_bridge[0]->get_node()->get_rigid_body();

			//red planks
			for (int i = 1; i != numPlanks - 1; i++)
			{
				mat.loadIdentity();
				mat.translate((i * 2) - 7, -10, 0);
				m_bridge[i] = app_scene->add_shape(mat, new mesh_box(vec3(1.4f, 0.1f, 5)), red, true);
				rb_bridge[i] = m_bridge[i]->get_node()->get_rigid_body();
			}

			//final green plank
			mat.loadIdentity();
			mat.translate(rPlank_xInt, rPlank_yInt, 0);
			m_bridge[numPlanks - 1] = app_scene->add_shape(mat, new mesh_box(vec3(0.1f, 0.1f, 10)), green, false);
			rb_bridge[numPlanks - 1] = m_bridge[numPlanks - 1]->get_node()->get_rigid_body();

			createSpring();
		}

		void createSpring()
		{
			btTransform plank1 = btTransform::getIdentity();
			plank1.setOrigin(btVector3(-1.8f, 0, 0));

			btTransform plank2 = btTransform::getIdentity();
			plank2.setOrigin(btVector3(1.8f, 0, 0));

			for (int i = 0; i < numPlanks - 1; i++)
			{
				app_scene->shapeSpring(rb_bridge[i], rb_bridge[i + 1], plank2, plank1);
			}

		}

		void simulate()
		{

			if (is_key_going_down(key_space))
			{
				mat4t mat;

			/*	mat.loadIdentity();
				mat.translate(0, 0, 0);
				app_scene->add_shape(mat, new mesh_sphere(vec3(2, 2, 2), 2), blue, true);

				ALuint source = get_sound_source();
				alSourcei(source, AL_BUFFER, warp1);
				alSourcePlay(source);
				*/

			}

			if (is_key_going_down(key('D')))
			{
				lPlank_xInt++;
			}
			if (is_key_going_down(key('A')))
			{
				lPlank_xInt--;
			}
			if (is_key_going_down(key('W')))
			{
				lPlank_yInt++;
			}
			if (is_key_going_down(key('S')))
			{
				lPlank_yInt--;
			}
			//
			//
			if (is_key_going_down(key_right))
			{
				rPlank_xInt++;
			}
			if (is_key_going_down(key_left))
			{
				rPlank_xInt--;
			}
			if (is_key_going_down(key_up))
			{
				rPlank_yInt++;
			}
			if (is_key_going_down(key_down))
			{
				rPlank_yInt--;
			}


			m_bridge[0]->get_node()->set_position(vec3(lPlank_xInt, lPlank_yInt, 0));
			m_bridge[numPlanks - 1]->get_node()->set_position(vec3(rPlank_xInt, rPlank_yInt, 0));


			///an attempt to get the camera to move with the planks...

			//app_scene->get_camera_instance(0)->get_node()->set_transform(
			//	((lPlank_xInt + rPlank_xInt)*0.5f, (lPlank_yInt + rPlank_yInt)*0.5f, numPlanks + (numPlanks*0.5f)));

			//vec3 asdf = app_scene->get_camera_instance(0)->get_node()->get_position();
			//printf("\n %i,", asdf);

		}



		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			simulate();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}


	};
}
