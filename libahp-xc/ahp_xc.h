/**
* \license
*    MIT License
*
*    libahp_xc library to drive the AHP XC correlators
*    Copyright (C) 2022  Ilia Platone
*
*    Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in all
*    copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*    SOFTWARE.
*/

#ifndef _AHP_XC_H
#define _AHP_XC_H

#ifdef  __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT extern
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* \mainpage AHP® XC Crosscorrelators driver library
* \section Introduction
*
* The AHP XC correlators do cross-correlation and auto-correlation
* counting from quantum detectors, ranging from radio to photon counters to geiger-mode detectors
* or noise-scattering sensors. The XC series offer a scientific grade solution for
* laboratory testing and measurement in quantum resoluting detection environments.
*
* This software is meant to work with the XC series cross-correlators
* visit https://www.iliaplatone.com/xc for more informations and purchase options.
*
* \author Ilia Platone
* \version 1.3.0
* \date 2017-2022
* \copyright MIT License.
*/

/**
 * \defgroup XC_API AHP® XC Correlators API Documentation
 *
 * This library contains functions for direct low-level usage of the AHP cross-correlators.<br>
 *
 * This documentation describes utility, applicative and hardware control functions included into the library.<br>
 * Each section and component is documented for general usage.
*/
/**\{*/

/**
 * \defgroup Defs Defines
 */
 /**\{*/

///This library version
#define AHP_XC_VERSION 0x130
///The base baud rate of the XC cross-correlators
#define XC_BASE_RATE ((int)57600)
///The base baud rate for big packet XC cross-correlators
#define XC_HIGH_RATE ((int)500000)
///The PLL frequency of the XC cross-correlators
#define AHP_XC_PLL_FREQUENCY 400000000

/**\}
 * \defgroup Types
 *\{*/

///AHP XC header flags
typedef enum {
///Indicates that the correlator can cross-correlate its inputs
HAS_CROSSCORRELATOR = 1,
///Indicates if the correlator has led lines available to drive
HAS_LEDS = 2,
///Indicates that the correlator has an internal PSU PWM driver on 2nd flag bit
HAS_PSU = 4,
///Indicates that the correlator has differential correlators only
HAS_DIFFERENTIAL_ONLY = 8
} xc_header_flags;

/**
* \brief Baud rate multipliers
*/
typedef enum {
    R_BASE = 0,
    R_BASEX2 = 1,
    R_BASEX4 = 2,
    R_BASEX8 = 3,
} baud_rate;

/**
* \brief The XC firmare commands
*/
typedef enum {
///Clear autocorrelation and crosscorrelation delays
CLEAR = 0,
///Set the current input line index for following commands
SET_INDEX = 1,
///Set on or off current line leds, requires HAS_LEDS
SET_LEDS = 2,
///Set the readout and command baud rate
SET_BAUD_RATE = 3,
///Set the autocorrelator or crosscorrelator delay
SET_DELAY = 4,
///Set the frequency divider in powers of two
SET_FREQ_DIV = 8,
///Set the indexed input voltage, requires HAS_PSU in header
SET_VOLTAGE = 9,
///Enables tests on current input
ENABLE_TEST = 12,
///Enable capture flags
ENABLE_CAPTURE = 13
} xc_cmd;

/**
* \brief The XC capture flags
*/
typedef enum {
///No extra signals or functions
CAP_NONE = 0,
///Enable capture
CAP_ENABLE = 1,
///Enable external clock
CAP_EXT_CLK = 2,
///Reset timestamp
CAP_RESET_TIMESTAMP = 4,
///Enable extra commands
CAP_EXTRA_CMD = 8,
///All flags enabled
CAP_ALL = 0xf,
} xc_capture_flags;

/**
* \brief The XC firmare commands
*/
typedef enum {
///No extra signals or functions
TEST_NONE = 0,
///Apply PLL clock on voltage led
TEST_SIGNAL = 1,
///Autocorrelator continuum scan
SCAN_AUTO = 2,
///Crosscorrelator continuum scan
SCAN_CROSS = 4,
///BCM modulation on voltage led
TEST_BCM = 8,
///All tests enabled
TEST_ALL = 0xf,
} xc_test_flags;

/**
* \brief Correlations structure
*/
typedef struct {
///Time lag offset
double lag;
///I samples count
long real;
///Q samples count
long imaginary;
///Pulses count
unsigned long counts;
///Magnitude of this sample
double magnitude;
///Phase of this sample
double phase;
} ahp_xc_correlation;

/**
* \brief Sample structure
*/
typedef struct {
///Lag offset from sample time
double lag;
///Maximum lag in a single shot
unsigned long lag_size;
///Correlations array, of size lag_size in an ahp_xc_packet
ahp_xc_correlation *correlations;
} ahp_xc_sample;

/**
* \brief Packet structure
*/
typedef struct {
///Timestamp of the packet
unsigned long timestamp;
///Number of lines in this correlator
unsigned long n_lines;
///Total number of baselines obtainable
unsigned long n_baselines;
///Bandwidth inverse frequency
unsigned long tau;
///Bits capacity in each sample
unsigned long bps;
///Crosscorrelators channels per packet
unsigned long cross_lag;
///Autocorrelators channels per packet
unsigned long auto_lag;
///Counts in the current packet
unsigned long* counts;
///Autocorrelations in the current packet
ahp_xc_sample* autocorrelations;
///Crosscorrelations in the current packet
ahp_xc_sample* crosscorrelations;
///Packet buffer string
const char* buf;
} ahp_xc_packet;

/**\}*/
/**
 * \defgroup Utilities Utility functions
*/
/**\{*/

/**
* \brief Get 2d projection for intensity interferometry
* \param alt The altitude coordinate
* \param az The azimuth coordinate
* \param baseline The reference baseline in meters
* \return Returns a 3-element double vector containing the 2d perspective coordinates and the z-offset
*/
DLL_EXPORT double* ahp_xc_get_2d_projection(double alt, double az, double *baseline);

/**\}*/
/**
 * \defgroup Comm Communication
*/
/**\{*/

/**
* \brief Connect to a serial port
* \param port The serial port name or filename
* \param high_rate The correlator needs high-speed communicator rate
* \return Returns non-zero on failure
* \sa ahp_xc_disconnect
*/
DLL_EXPORT int ahp_xc_connect(const char *port, int high_rate);

/**
* \brief Connect to a serial port or other stream associated to the given file descriptor
* \param fd The file descriptor of the stream
* \return Returns non-zero on failure
*/
DLL_EXPORT int ahp_xc_connect_fd(int fd);

/**
* \brief Obtain the serial port file descriptor
* \return The file descriptor of the stream
*/
DLL_EXPORT int ahp_xc_get_fd();

/**
* \brief Disconnect from the serial port or descriptor opened with ahp_xc_connect
* \sa ahp_xc_connect
*/
DLL_EXPORT void ahp_xc_disconnect(void);

/**
* \brief Report connection status
* \sa ahp_xc_connect
* \sa ahp_xc_connect_fd
* \sa ahp_xc_disconnect
* \return Returns non-zero if connected
*/
DLL_EXPORT unsigned int ahp_xc_is_connected(void);

/**
* \brief Report if a correlator was detected
* \sa ahp_xc_connect
* \sa ahp_xc_connect_fd
* \sa ahp_xc_disconnect
* \return Returns non-zero if a correlator was detected
*/
DLL_EXPORT unsigned int ahp_xc_is_detected(void);

/**
* \brief Obtain the current baud rate
* \return Returns the baud rate
*/
DLL_EXPORT int ahp_xc_get_baudrate(void);

/**
* \brief Obtain the current baud rate
* \param rate The new baud rate index
*/
DLL_EXPORT void ahp_xc_set_baudrate(baud_rate rate);

/**\}*/
/**
 * \defgroup Feat Features of the correlator
*/
/**\{*/

/**
* \brief Probe for a correlator and take its properties
* \return Returns non-zero on failure
*/
DLL_EXPORT int ahp_xc_get_properties(void);

/**
* \brief Obtain the correlator header
* \return Returns a string representing the correlator ID
*/
DLL_EXPORT char* ahp_xc_get_header(void);

/**
* \brief Obtain the correlator bits per sample
* \return Returns the bits per sample value
*/
DLL_EXPORT unsigned int ahp_xc_get_bps(void);

/**
* \brief Obtain the correlator number of lines
* \return Returns the number of lines
*/
DLL_EXPORT unsigned int ahp_xc_get_nlines(void);

/**
* \brief Obtain the correlator total baselines
* \return Returns the baselines quantity
*/
DLL_EXPORT unsigned int ahp_xc_get_nbaselines(void);

/**
* \brief Obtain the correlator maximum delay value
* \return Returns the delay size
*/
DLL_EXPORT unsigned int ahp_xc_get_delaysize(void);

/**
* \brief Obtain the correlator lag buffer size for autocorrelations
* \return Returns the lag size
*/
DLL_EXPORT unsigned int ahp_xc_get_autocorrelator_lagsize(void);

/**
* \brief Obtain the correlator lag buffer size for crosscorrelations
* \return Returns the lag size
*/
DLL_EXPORT unsigned int ahp_xc_get_crosscorrelator_lagsize(void);

/**
* \brief Obtain the correlator maximum readout frequency
* \return Returns the maximum readout frequency
*/
DLL_EXPORT unsigned int ahp_xc_get_frequency(void);

/**
* \brief Obtain the correlator clock divider
* \return Returns the clock divider (powers of 2)
*/
DLL_EXPORT unsigned int ahp_xc_get_frequency_divider(void);

/**
* \brief Obtain the sampling time
* \return Returns the sampling time in nanoseconds
*/
DLL_EXPORT double ahp_xc_get_sampletime(void);

/**
* \brief Obtain the serial packet transmission time
* \return Returns the packet transmission time in microseconds
*/
DLL_EXPORT unsigned int ahp_xc_get_packettime(void);

/**
* \brief Obtain the serial packet size
* \return Returns the packet size in bytes
*/
DLL_EXPORT unsigned int ahp_xc_get_packetsize(void);

/**
* \brief Returns the cross-correlation capability of the device
* \return Returns non-zero if the device is a crosscorrelator
*/
DLL_EXPORT int ahp_xc_has_crosscorrelator(void);

/**
* \brief Returns if the device offers internal PSU line
* \return Returns non-zero if PSU is available
*/
DLL_EXPORT int ahp_xc_has_psu(void);

/**
* \brief Returns if the device has led lines to drive
* \return Returns non-zero if leds are available
*/
DLL_EXPORT int ahp_xc_has_leds(void);

/**
* \brief Returns if the device has differential correlators only
* \return Returns non-zero if the device is a differential correlator only
*/
DLL_EXPORT int ahp_xc_has_differential_only();

/**\}*/
/**
 * \defgroup Data Data and streaming
*/
/**\{*/

/**
* \brief Allocate and return a packet structure
* \return Returns a new ahp_xc_packet structure pointer
*/
DLL_EXPORT ahp_xc_packet *ahp_xc_alloc_packet(void);

/**
* \brief Free a previously allocated packet structure
* \param packet pointer to the ahp_xc_packet structure to be freed
*/
DLL_EXPORT void ahp_xc_free_packet(ahp_xc_packet *packet);

/**
* \brief Allocate and return a samples array
* \param nlines The Number of samples to be allocated.
* \param len The lag_size and correlations field size of each sample.
* \return Returns the new allocated ahp_xc_sample array
* \sa ahp_xc_free_samples
* \sa ahp_xc_sample
* \sa ahp_xc_packet
*/
DLL_EXPORT ahp_xc_sample *ahp_xc_alloc_samples(unsigned long nlines, unsigned long len);

/**
* \brief Free a previously allocated samples array
* \param nlines The Number of samples to be allocated.
* \param samples the ahp_xc_sample array to be freed
* \sa ahp_xc_alloc_samples
* \sa ahp_xc_sample
* \sa ahp_xc_packet
*/
DLL_EXPORT void ahp_xc_free_samples(unsigned long nlines, ahp_xc_sample *samples);

/**
* \brief Grab a data packet
* \param packet The ahp_xc_packet structure to be filled.
* \return Returns non-zero on error
* \sa ahp_xc_set_channel_auto
* \sa ahp_xc_set_channel_cross
* \sa ahp_xc_alloc_packet
* \sa ahp_xc_free_packet
* \sa ahp_xc_packet
*/
DLL_EXPORT int ahp_xc_get_packet(ahp_xc_packet *packet);

/**
* \brief Initiate an autocorrelation scan
* \param index The line index.
* \param start The starting channel for this scan.
* \param size The number of channels to scan afterwards.
*/
DLL_EXPORT void ahp_xc_start_autocorrelation_scan(unsigned int index, off_t start, size_t size);

/**
* \brief End an autocorrelation scan
* \param index The line index.
*/
DLL_EXPORT void ahp_xc_end_autocorrelation_scan(unsigned int index);

/**
* \brief Scan all available delay channels and get autocorrelations of each input
* \param index the input index.
* \param autocorrelations An ahp_xc_sample array pointer, can be NULL. Will be allocated by reference and filled by this function.
* \param start First channel to be scanned.
* \param len Number of channels to be scanned.
* \param interrupt This should point to an int32_t variable, when setting to 1, on a separate thread, scanning will be interrupted.
* \param percent Like interrupt a variable, passed by reference that will be updated with the percent of completion.
* \return Returns the number of channels scanned
* \sa ahp_xc_get_delaysize
* \sa ahp_xc_sample
*/
DLL_EXPORT int ahp_xc_scan_autocorrelations(unsigned int index, ahp_xc_sample **autocorrelations, off_t start, unsigned int len, int *interrupt, double *percent);

/**
* \brief Initiate a crosscorrelation scan
* \param index The line index.
* \param start the starting channel for this scan.
*/
DLL_EXPORT void ahp_xc_start_crosscorrelation_scan(unsigned int index, off_t start, size_t size);

/**
* \brief End a crosscorrelation scan
* \param index The line index.
*/
DLL_EXPORT void ahp_xc_end_crosscorrelation_scan(unsigned int index);

/**
* \brief Scan all available delay channels and get crosscorrelations of each input with others
* \param index1 the first input index.
* \param index2 the second input index.
* \param crosscorrelations An ahp_xc_sample array pointer, can be NULL. Will be allocated by reference and filled by this function.
* \param start1 Initial channel on index1 input.
* \param start2 Initial channel on index2 input.
* \param size Number of channels to be scanned.
* \param interrupt This should point to an int32_t variable, when setting to 1, on a separate thread, scanning will be interrupted.
* \param percent Like interrupt a variable, passed by reference that will be updated with the percent of completion.
* \return Returns the number of channels scanned
* \sa ahp_xc_get_delaysize
* \sa ahp_xc_sample
*/
DLL_EXPORT int ahp_xc_scan_crosscorrelations(unsigned int index1, unsigned int index2, ahp_xc_sample **crosscorrelations, off_t start1, off_t start2, unsigned int size, int *interrupt, double *percent);

/**\}*/
/**
 * \defgroup Cmds Commands and setup of the correlator
*/
/**\{*/

/**
* \brief Set integration flags
* \param flags New capture flags mask.
*/
DLL_EXPORT int ahp_xc_set_capture_flags(xc_capture_flags flags);

/**
* \brief Get integration flags
* \return current capture flags.
*/
DLL_EXPORT xc_capture_flags ahp_xc_get_capture_flags();

/**
* \brief Switch on or off the led lines of the correlator
* \param index The input line index starting from 0
* \param leds The enable mask of the leds
*/
DLL_EXPORT void ahp_xc_set_leds(unsigned int index, int leds);

/**
* \brief Set the channel of the selected input (for cross-correlation)
* \param index The input line index starting from 0
* \param value The channel number
* \param size The number of channels to scan afterwards
*/
DLL_EXPORT void ahp_xc_set_channel_cross(unsigned int index, off_t value, size_t size);

/**
* \brief Set the channel of the selected input (for auto-correlation)
* \param index The input line index starting from 0
* \param value The channel number
* \param size The number of channels to scan afterwards
*/
DLL_EXPORT void ahp_xc_set_channel_auto(unsigned int index, off_t value, size_t size);

/**
* \brief Set the clock divider for autocorrelation and crosscorrelation
* \param value The clock divider power of 2
*/
DLL_EXPORT void ahp_xc_set_frequency_divider(unsigned char value);

/**
* \brief Set the supply voltage on the current line
* \param index The input line index starting from 0
* \param value The voltage level
*/
DLL_EXPORT void ahp_xc_set_voltage(unsigned int index, unsigned char value);

/**
* \brief Enable tests on the current line
* \param index The input line index starting from 0
* \param value The test type
*/
DLL_EXPORT void ahp_xc_set_test_flags(unsigned int index, xc_test_flags value);

/**
* \brief Get the current status of the test features
* \param index The line index starting from 0
* \return The current tests on index input
*/
DLL_EXPORT unsigned char ahp_xc_get_test_flags(unsigned int index);

/**
* \brief Get the current status of the leds on line
* \param index The line index starting from 0
* \return The current led configuration on index input
*/
DLL_EXPORT unsigned char ahp_xc_get_leds(unsigned int index);

/**
* \brief Select the input on which to issue next command
* \param index The input index
*/
DLL_EXPORT void ahp_xc_select_input(unsigned int index);

/**
* \brief Returns the currently selected input on which next command will be issued
* \return The input index
*/
DLL_EXPORT unsigned int ahp_xc_current_input();

/**
* \brief Send an arbitrary command to the AHP XC device
* \param cmd The command
* \param value The command parameter
* \return non-zero on failure
*/
DLL_EXPORT int ahp_xc_send_command(xc_cmd cmd, unsigned char value);

/**
* \brief Obtain the current libahp-xc version
* \return The current version code
*/
DLL_EXPORT inline unsigned int ahp_xc_get_version(void) { return AHP_XC_VERSION; }

/**\}*/
/**\}*/
#ifdef __cplusplus
} // extern "C"
#endif

#endif //_AHP_XC_H
