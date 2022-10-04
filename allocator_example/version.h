
//********************************************************************************************************
// Public defines
//********************************************************************************************************

	#define VERSION_NAME 		"ML-BASE" 
	#define VERSION_NUMBERS 	6,0,0
	#define VERSION_STRING 		"V6.0.0b"

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//	String will be in PROGMEM for AVR platform
	extern const char	 	version_name[];			// "Name-of-product"
	extern const uint32_t 	version_build_number;	// 1-N
	extern const char		version_build_date[];	// "2019-06-02T01:38:59+00:00"
	extern const uint8_t	version_numbers[3];		// 0:major 1:minor 2:patch
	extern const char		version_string[];		// "V0.0.0"
	extern const char		version_gcc_version[];	// "gcc (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"   (first line of cc --version)

