#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <frc/Joystick.h>
#include <frc/XboxController.h>

#include "Toggle.h"

namespace curtinfrc {
  typedef std::pair<int, int> tControllerButton;
  constexpr tControllerButton noButton{ -1, -1 };
  typedef std::vector<tControllerButton> tControllerButtonMap;

  typedef std::pair<int, int> tControllerAxis;
  constexpr tControllerAxis noAxis{ -1, -1 };
  // typedef std::vector<tControllerAxis> tControllerAxisMap;

  template <class ContType>
  class Controller {
   public:
    Controller(int port, int nButtons = 12) : _cont(port), _nButtons(nButtons), _buttonRiseToggle(nButtons, Toggle(ToggleEvent::ONRISE)), _buttonFallToggle(nButtons, Toggle(ToggleEvent::ONFALL, false)) {
      (void)static_cast<frc::GenericHID*>((ContType*)0); // ContType must be an frc::GenericHID
    };

    /* virtual */ int GetButtonCount() const { return _nButtons; };

    int CurrentPort() { return _cont.GetPort(); };
    double GetRawAxis(int axis) { return _cont.GetRawAxis(axis); };
    bool GetRawButton(int button) { return _cont.GetRawButton(button); };

    virtual double GetAxis(int axis) { return GetRawAxis(axis); };
    virtual bool GetButton(int button) { return GetRawButton(button); };

    virtual bool GetButtonRise(int button) { return _buttonRiseToggle[button - 1].Update(GetButton(button)); };
    virtual bool GetButtonFall(int button) { return _buttonFallToggle[button - 1].Update(GetButton(button)); };

    // Joystick specific override
    virtual double GetCircularisedAxisAgainst(int primaryAxis, int compareAxis) { return GetCircularisedAxis(primaryAxis); };
    virtual double GetCircularisedAxis(int axis) { return GetAxis(axis); };

   protected:
    ContType _cont;

   private:
    const int _nButtons;
    std::vector<Toggle> _buttonRiseToggle, _buttonFallToggle;
  };

  class ControllerGroup {
   public:
    template <class... Controllers>
    explicit ControllerGroup(Controller<frc::GenericHID>& cont, Controllers&... conts) : m_conts{cont, conts...} {};

    ControllerGroup(ControllerGroup&&) = default;
    ControllerGroup& operator=(ControllerGroup&&) = default;

    double GetRawAxis(int cont, int axis);
    double GetAxis(tControllerAxis contAxis);

    double GetCircularisedAxisAgainst(int cont, int primaryAxis, int compareAxis);
    double GetCircularisedAxisAgainst(tControllerAxis primaryAxis, tControllerAxis compareAxis);
    double GetCircularisedAxis(int cont, int axis);
    double GetCircularisedAxis(tControllerAxis axis);


    bool GetRawButton(int cont, int button);
    bool GetRawButtonRise(int cont, int button);
    bool GetRawButtonFall(int cont, int button);

    bool GetButton(tControllerButton pair);
    bool GetButtonRise(tControllerButton pair);
    bool GetButtonFall(tControllerButton pair);

    bool GetButton(tControllerButtonMap map);
    bool GetButtonRise(tControllerButtonMap map);
    bool GetButtonFall(tControllerButtonMap map);

    Controller<frc::GenericHID> &GetController(int cont);

   private:
    std::vector<std::reference_wrapper<Controller<frc::GenericHID>>> m_conts;
  };

  class Joystick : public Controller<frc::Joystick> {
   public:
    Joystick(int port) : Controller(port, 12) {};
    enum JoyAxis {
      kXAxis = 0,
      kYAxis = 1,
      kZAxis = 2, kTwistAxis = 2,
      kThrottleAxis = 3
    };

    virtual double GetCircularisedAxisAgainst(int primaryAxis, int compareAxis) override;
    virtual double GetCircularisedAxis(int axis) override;
  };

  class XboxController : public Controller<frc::XboxController> {
   public:
    XboxController(int port) : Controller(port, 12) {};

    enum XboxAxis {
      kLeftXAxis = 0,
      kLeftYAxis = 1,
      // Throttles?
      kRightXAxis = 4,
      kRightYAxis = 5,
    };

    using XboxButton = frc::XboxController::Button;
  };
} // ns curtinfrc
