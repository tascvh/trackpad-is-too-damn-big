/*
 *
 * This file is part of trackpad-is-too-damn-big utility
 * Copyright (c) https://github.com/tascvh/trackpad-is-too-damn-big
 *
 */
#include "devices.hpp"
#include "event_filters.hpp"
#include "event_handlers.hpp"
#include "simple_parser.hpp"
#include <iostream>
#include <optional>

int main(int argc, char **argv) {

  struct RunningMode {
    enum class Type { Print, Strict, Flex, Invalid };
    static Type FromString(const std::string &mode) {
      if (mode == "p")
        return Type::Print;
      if (mode == "s")
        return Type::Strict;
      if (mode == "f")
        return Type::Flex;
      return Type::Invalid;
    }
  };

  try {
    // cmd arguments
    std::optional<std::string> device;
    std::string mode("f");
    int left(10);
    int right(10);
    int top(0);
    int bottom(15);

    {
      SimpleParser sp(argc, argv);
      sp.read(sp.m_showHelp, "-h", "");
      if (sp.m_showHelp) {
        std::cout << "-h : Show this help message" << std::endl;
      }

      sp.read(device, "-d", "trackpad device filename (mandatory)");
      sp.read(mode, "-m", "Running mode options : p/s/f ");
      sp.read(left, "-l", "left percentage");
      sp.read(right, "-r", "right percentage");
      sp.read(top, "-t", "top percentage");
      sp.read(bottom, "-b", "bottom percentage");

      if (sp.m_showHelp) {
        std::cout << std::endl << "Example usage :" << std::endl;
        std::cout << "\tsudo ./titdb -d /dev/input/event0 -m f" << std::endl;
        return EXIT_SUCCESS;
      }
    }

    if (!device) {
      auto s = "device argument is mandatory. Use -h for help";
      throw std::invalid_argument(s);
    }

    auto evdev = Evdev(*device);

    switch (RunningMode::FromString(mode)) {
    case RunningMode::Type::Print: {
      evdev.print();
      return evdev.runEventLoop(PrintEvents());
    }
    case RunningMode::Type::Strict: {
      return evdev.runEventLoop(
          ForwardTo(evdev.Spawn<UInput>(),
                    evdev.Spawn<CropRect>(left, right, top, bottom)));
    }
    case RunningMode::Type::Flex: {
      return evdev.runEventLoop(
          ForwardTo(evdev.Spawn<UInput>(),
                    evdev.Spawn<CropRectFlex>(left, right, top, bottom)));
    }
    case RunningMode::Type::Invalid: {
      auto s = std::format("Invalid running mode {}. Use -h for help", mode);
      throw std::invalid_argument(s);
    }
    }

  } catch (const std::exception &e) {
    std::cerr << "error: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
  }

  return EXIT_FAILURE;
}
