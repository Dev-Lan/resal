#include "App.hpp"

using namespace std;

App::App(const GApp::Settings& settings) : GApp(settings) {
  renderDevice->setColorClearValue(Color3(0.8, 0.8, 0.8));
}

void App::onInit() {
  GApp::onInit();

  // Typical G3D::GApp initialization options
  // createDeveloperHUD();
  // debugWindow->setVisible(false);
  // developerWindow->setVisible(false);
  // developerWindow->cameraControlWindow->setVisible(false);
  // showRenderingStats = false;

  if (DEBUG_ENABLED) {
    std::cout << "debug defined" << std::endl;
    createDeveloperHUD();
    debugWindow->setVisible(false);
    developerWindow->setVisible(false);
    developerWindow->cameraControlWindow->setVisible(false);
    developerWindow->sceneEditorWindow->setVisible(false);
    activeCamera()->setFrame(CFrame::fromXYZYPRDegrees(  0.9f,   0.0f,  51.6f,   0.0f,  -0.0f,   0.0f));
  } else {
    activeCamera()->setFrame(CFrame::fromXYZYPRDegrees(  0.9f,   0.0f,  42.6f,   0.0f,  -0.0f,   0.0f));
  }
  showRenderingStats = false;

  // shared_ptr< Texture >  groundTexture    = Texture::fromFile(RUSTY_DIAMOND_METAL_TEXTURE_FILENAME);
  // shared_ptr< Texture >  groundNrmTexture = Texture::fromFile(RUSTY_DIAMOND_METAL_NRM_FILENAME);

  // shared_ptr< Texture >  groundTexture = Texture::fromFile(NRM_TEST_FILENAME);
  // shared_ptr< Texture >  groundNrmTexture = groundTexture;

  // shared_ptr< Texture >  groundTexture    = Texture::fromFile(SHINY_METAL_HANDLE_TEXTURE_FILENAME);
  // shared_ptr< Texture >  groundNrmTexture = Texture::fromFile(SHINY_METAL_HANDLE_NRM_FILENAME);

  shared_ptr< Texture >  groundTexture    = Texture::fromFile(SHEET_METAL_TEXTURE_FILENAME);
  shared_ptr< Texture >  groundNrmTexture = Texture::fromFile(SHEET_METAL_NRM_FILENAME);

  // shared_ptr< Texture >  groundTexture    = Texture::fromFile(PAPER_TEXTURE_FILENAME);
  // shared_ptr< Texture >  groundNrmTexture = Texture::fromFile(PAPER_NRM_FILENAME);



  float width  = 100; // in centimeters
  float height = width * groundTexture->height() / groundTexture->width();
  _levels = new Levels(LEVELS_FILENAME);
  _laserEngine = new LaserEngine(width, height, groundTexture, groundNrmTexture, _levels->currentLevel(), renderDevice);
}


void App::onUserInput(UserInput *ui) {
  GApp::onUserInput(ui); // needed for widgets to advance (camera manipulators, GUIs)
  _laserEngine->onUserInput(ui, activeCamera());
  // if (ui->keyDown(GKey::LEFT_MOUSE)) {
  //  //  more like liberal propaganda mouse, amirite
  //   }
  // }

  if (ui->keyPressed(GKey('.'))){
    _laserEngine->loadLevel(_levels->nextLevel());
  }

  if (ui->keyPressed(GKey(','))){
    _laserEngine->loadLevel(_levels->previousLevel());
  }

}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
  GApp::onSimulation(rdt, sdt, idt); // need for widgets to advance (camera manipulators, GUIs)
  _laserEngine->onSimulation(rdt);
}



void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) {
  // rd->clear();
  // Sphere s(1);
  // Draw::sphere(s,rd);
  
  swapBuffers();
  rd->clear();
  _laserEngine->onGraphics3D(rd);

  // _groundPlane->d_lightColor = d_rainbowTex->readTexel(d_rainbowIndex,10).rgb();
  // _groundPlane->draw(rd);
  // d_rainbowIndex = d_rainbowIndex % d_rainbowTex->width();

}


void App::onGraphics2D(RenderDevice* rd, Array<Surface2D::Ref>& posed2D) {
    _laserEngine->onGraphics2D(rd);

  // Surface2D::sortAndRender(rd, posed2D);
  
  // Array<Vector2> circle;
  // int numberOfVerts = 20;
  // Vector2 center(500,150);
  // double tau = 6.28;
  // double radius = 100;
  // for(int i=0; i<numberOfVerts; ++i){
  //   double percent = (double) i / (double) numberOfVerts;
  //   double x = radius*sin(percent*tau) + center.x;
  //   double y = radius*cos(percent*tau) + center.y;
  //   circle.append(Vector2(x,y));
  // }
  // Draw::poly2D(circle, rd, Color3::blue());
}