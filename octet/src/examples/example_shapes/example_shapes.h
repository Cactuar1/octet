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


	int numPlanks = 20;
	mesh_instance *m_bridge[20];
	btRigidBody *rb_bridge[20];

	float plankDamping = 0.002f;	//guess lol

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }


    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, numPlanks));


      material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
	  material *purple = new material(vec4(1, 0, 1, 1));
	  material *white = new material(vec4(1, 1, 1, 1));

      mat4t mat;
      // ground
     // mat.loadIdentity();
    //  mat.translate(0, -3, 0);
     // app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), blue, false);
	  
	  

	  //tings
	 
	  mat.loadIdentity();
	  mat.translate((-numPlanks+1), 2, 0);
	  m_bridge[0] = app_scene->add_shape(mat, new mesh_box(vec3(0.1f, 0.1f, 10)), green, false);
	  rb_bridge[0] = m_bridge[0]->get_node()->get_rigid_body();


	  for (int i = 1; i != numPlanks-1; i++)
	  {
		  mat.loadIdentity();
		  mat.translate((i * 2) - 7, -4, 0);
		  m_bridge[i] = app_scene->add_shape(mat, new mesh_box(vec3(1.4f, 0.1f, 5)), red, true);
		  rb_bridge[i] = m_bridge[i]->get_node()->get_rigid_body();
	  }

	  mat.loadIdentity();
	  mat.translate((numPlanks-1), 2, 0);
	  m_bridge[numPlanks - 1] = app_scene->add_shape(mat, new mesh_box(vec3(0.1f, 0.1f, 10)), green, false);
	  rb_bridge[numPlanks - 1] = m_bridge[numPlanks - 1]->get_node()->get_rigid_body();


	  createSpring();

    }

	void createSpring()
	{
		btTransform tran1 = btTransform::getIdentity();
		tran1.setOrigin(btVector3(-1.5f, 0, 0));

		btTransform tran2 = btTransform::getIdentity();
		tran2.setOrigin(btVector3(1.5f, 0, 0));

		for (int i = 0; i < numPlanks-1; i++)
		{
			app_scene->applySpring(rb_bridge[i], rb_bridge[i + 1], tran1, tran2);
		}

	}

	void simulate()
	{
		material *blue = new material(vec4(0, 0, 1, 1));

		if (is_key_going_down(key_space))
		{
			mat4t mat;

			mat.loadIdentity();
			mat.translate(0, 0, 0);
			app_scene->add_shape(mat, new mesh_sphere(vec3(2, 2, 2), 2), blue, true);
		}
	}
	
	

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
		simulate();

      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }

	
  };
}
