/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2016-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_F429ZI_HPP
#define MODM_STM32_NUCLEO_F429ZI_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_f429zi
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_f429zi
namespace Board
{
	using namespace modm::literals;

/// STM32F429 running at 180MHz from the external 8MHz STLink clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 180_MHz;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static bool inline
	enable()
	{
		Rcc::enableExternalClock(); // 8 MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 4,		//   8MHz / M=  4 ->   2MHz
			.pllN = 180,	//   2MHz * N=180 -> 360MHz
			.pllP = 2,		// 360MHz / P=  2 -> 180MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalClock, pllFactors);
		PWR->CR |= PWR_CR_ODEN; // Enable overdrive mode
		while (not (PWR->CSR & PWR_CSR_ODRDY)) ;
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		// update clock frequencies
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputB14;	// LED3 [Red]
using Leds = SoftwareGpioPort< LedRed, LedBlue, LedGreen >;

namespace stlink
{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


inline void
initialize()
{
    SystemClock::enable();
    SysTickTimer::initialize<SystemClock>();

    stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
    stlink::Uart::initialize<SystemClock, 115200_Bd>();

    LedGreen::setOutput(modm::Gpio::Low);
    LedBlue::setOutput(modm::Gpio::Low);
    LedRed::setOutput(modm::Gpio::Low);

    Button::setInput();
    Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
    Button::enableExternalInterrupt();
//  Button::enableExternalInterruptVector(12);
}

}

#endif  // MODM_STM32_NUCLEO_F429ZI_HPP
