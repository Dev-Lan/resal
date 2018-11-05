#pragma once
#include <iostream>
#include <sstream>
#include <G3D/G3DAll.h>
#include "LaserEngine.hpp"
#include "Levels.hpp"
#include "config.hpp"


class App : public GApp {
public:

  App(const GApp::Settings& settings = GApp::Settings());
  
  virtual void onInit() override;

  virtual void onUserInput(UserInput *ui) override;
  virtual bool onEvent(const GEvent& e) override;
  virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;

  virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
  virtual void onGraphics2D(RenderDevice* rd, Array<Surface2D::Ref>& surface2D) override;

private:
  LaserEngine* _laserEngine;
  Levels* _levels;
};
