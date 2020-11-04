#pragma once

#include <usb/libusb++.hpp>
#include <usb/endpoint.hpp>

#include <avr/memory/pgmspace.hpp>

#include <utility>
#include <array>

namespace AVR::USB
{
	class Descriptor
	{
		public:
		const uint8_t *const m_ptr;
		uint8_t bLength() const { return m_ptr[0]; };
		DescriptorType bDescriptorType() const { return static_cast<DescriptorType>(m_ptr[1]); };
		protected:
		constexpr Descriptor(const uint8_t *const ptr) : m_ptr(ptr) 
		{
		}
	};

	class DeviceDescriptor : public Descriptor
	{
		const uint8_t m_buf[18];
		public:
		constexpr DeviceDescriptor(
				USB_BCD _bcdUSB, 
				DeviceClass _bDeviceClass,
				uint8_t _bDeviceSubClass,
				uint8_t _bDeviceProtocol,
				uint8_t _bMaxPacketSize0,
				uint16_t _idVendor,
				uint16_t _idProduct,
				uint16_t _bcdDevice,
				uint8_t _iManufacturer,
				uint8_t _iProduct,
				uint8_t _iSerialNumber,
				uint8_t _bNumConfigurations) : 
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x12),
				static_cast<uint8_t>(DescriptorType::Device),
				static_cast<uint8_t>(static_cast<uint16_t>(_bcdUSB)&0xFF),
				static_cast<uint8_t>(static_cast<uint16_t>(_bcdUSB)>>8),
				static_cast<uint8_t>(_bDeviceClass),
				static_cast<uint8_t>(_bDeviceSubClass),
				static_cast<uint8_t>(_bDeviceProtocol),
				static_cast<uint8_t>(_bMaxPacketSize0),
				static_cast<uint8_t>(_idVendor&0xFF),
				static_cast<uint8_t>(_idVendor>>8),
				static_cast<uint8_t>(_idProduct&0xFF),
				static_cast<uint8_t>(_idProduct>>8),
				static_cast<uint8_t>(_bcdDevice&0xFF),
				static_cast<uint8_t>(_bcdDevice>>8),
				static_cast<uint8_t>(_iManufacturer),
				static_cast<uint8_t>(_iProduct),
				static_cast<uint8_t>(_iSerialNumber),
				static_cast<uint8_t>(_bNumConfigurations),
			} {}
		USB_BCD bcdUSB() { return static_cast<USB_BCD>(m_buf[2] | m_buf[3]<<8); }
		DeviceClass bDeviceClass() { return static_cast<DeviceClass>(m_buf[4]); }
		uint8_t bSubDeviceClass() { return static_cast<uint8_t>(m_buf[5]); }
		uint8_t bDeviceProtocol() { return static_cast<uint8_t>(m_buf[6]); }
		uint8_t bMaxPacketSize0() { return static_cast<uint8_t>(m_buf[7]); }
		uint16_t idVendor() { return static_cast<uint16_t>(m_buf[8] | m_buf[9]<<8); }
		uint16_t idProduct() { return static_cast<uint16_t>(m_buf[10] | m_buf[11]<<8); }
		uint16_t bcdDevice() { return static_cast<uint16_t>(m_buf[12] | m_buf[13]<<8); }
		uint8_t iManufacturer() { return static_cast<uint8_t>(m_buf[14]); }
		uint8_t iProduct() { return static_cast<uint8_t>(m_buf[15]); }
		uint8_t iSerialNumber() { return static_cast<uint8_t>(m_buf[16]); }
		uint8_t numConfigurations() { return static_cast<uint8_t>(m_buf[17]); }
	};

	class ConfigurationDescriptor : public Descriptor
	{
		const uint8_t m_buf[9];
	public:
		constexpr ConfigurationDescriptor(
				uint16_t _wTotalLength,
				uint8_t _bNumInterfaces,
				uint8_t _bConfigurationValue,
				uint8_t _iConfiguration,
				ConfigurationAttributes _bmAttributes,
				Power _bMaxPower ) : 
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x09),
				static_cast<uint8_t>(DescriptorType::Configuration),
				static_cast<uint8_t>(_wTotalLength&0xFF),
				static_cast<uint8_t>(_wTotalLength>>8),
				static_cast<uint8_t>(_bNumInterfaces),
				static_cast<uint8_t>(_bConfigurationValue),
				static_cast<uint8_t>(_iConfiguration),
				static_cast<uint8_t>(_bmAttributes),
				static_cast<uint8_t>(_bMaxPower),
			} {}
		uint16_t wTotalLength() const { return static_cast<uint16_t>(m_buf[2] | m_buf[3]<<8); };
		uint8_t bNumInterfaces() const { return static_cast<uint8_t>(m_buf[4]); };
		uint8_t bConfigurationValue() const { return static_cast<uint8_t>(m_buf[5]); };
		uint8_t iConfiguration() const { return static_cast<uint8_t>(m_buf[6]); };
		uint8_t bmAttributes() const { return static_cast<uint8_t>(m_buf[7]); };
		Power bMaxPower() const { return static_cast<Power>(m_buf[8]); };
	};

	class InterfaceDescriptor : public Descriptor
	{
		const uint8_t m_buf[9];
	public:
		constexpr InterfaceDescriptor(
				uint8_t _bInterfaceNumber,
				uint8_t _bAlternateSetting,
				uint8_t _bNumEndpoints,
				InterfaceClass _bInterfaceClass,
				uint8_t _bInterfaceSubClass,
				uint8_t _bInterfaceProtocol,
				uint8_t _iInterface
			) :
			Descriptor(m_buf),
			m_buf{
				static_cast<uint8_t>(0x09),
				static_cast<uint8_t>(DescriptorType::Interface),
				static_cast<uint8_t>(_bInterfaceNumber),
				static_cast<uint8_t>(_bAlternateSetting),
				static_cast<uint8_t>(_bNumEndpoints),
				static_cast<uint8_t>(_bInterfaceClass),
				static_cast<uint8_t>(_bInterfaceSubClass),
				static_cast<uint8_t>(_bInterfaceProtocol),
				static_cast<uint8_t>(_iInterface),
			} {}
		uint8_t bInterfaceNumber() { return static_cast<uint8_t>(m_buf[2]); }
		uint8_t bAlternateSetting() { return static_cast<uint8_t>(m_buf[3]); }
		uint8_t bNumEndpoints() { return static_cast<uint8_t>(m_buf[4]); }
		uint8_t bInterfaceClass() { return static_cast<uint8_t>(m_buf[5]); }
		uint8_t bInterfaceSubClass() { return static_cast<uint8_t>(m_buf[6]); }
		uint8_t bInterfaceProtocol() { return static_cast<uint8_t>(m_buf[7]); }
		uint8_t iInterface() { return static_cast<uint8_t>(m_buf[8]); }
	};

	class EndpointDescriptor : public Descriptor
	{
		
	};

	enum class LanguageID : uint16_t{
		Afrikaans = 1078,
		Albanian = 1052,
		Amharic = 1118,
		Arabic_Algeria = 5121,
		Arabic_Bahrain = 15361,
		Arabic_Egypt = 3073,
		Arabic_Iraq = 2049,
		Arabic_Jordan = 11265,
		Arabic_Kuwait = 13313,
		Arabic_Lebanon = 12289,
		Arabic_Libya = 4097,
		Arabic_Morocco = 6145,
		Arabic_Oman = 8193,
		Arabic_Qatar = 16385,
		Arabic_Saudi_Arabia = 1025,
		Arabic_Syria = 10241,
		Arabic_Tunisia = 7169,
		Arabic_United_Arab_Emirates = 14337,
		Arabic_Yemen = 9217,
		Armenian = 1067,
		Assamese = 1101,
		Azeri_Cyrillic = 2092,
		Azeri_Latin = 1068,
		Basque = 1069,
		Belarusian = 1059,
		Bengali_Bangladesh = 2117,
		Bengali_India = 1093,
		Bosnian = 5146,
		Bulgarian = 1026,
		Burmese = 1109,
		Catalan = 1027,
		Chinese_China = 2052,
		Chinese_Hong_Kong_SAR = 3076,
		Chinese_Macau_SAR = 5124,
		Chinese_Singapore = 4100,
		Chinese_Taiwan = 1028,
		Croatian = 1050,
		Czech = 1029,
		Danish = 1030,
		// Divehi = dv,
		Dutch_Belgium = 2067,
		Dutch_Netherlands = 1043,
		Edo = 1126,
		English_Australia = 3081,
		English_Belize = 10249,
		English_Canada = 4105,
		English_Caribbean = 9225,
		English_Great_Britain = 2057,
		English_India = 16393,
		English_Ireland = 6153,
		English_Jamaica = 8201,
		English_New_Zealand = 5129,
		English_Philippines = 13321,
		English_Southern_Africa = 7177,
		English_Trinidad = 11273,
		English_United_States = 1033,
		English_Zimbabwe = 12297,
		Estonian = 1061,
		FYRO_Macedonia = 1071,
		Faroese = 1080,
		Farsi_Persian = 1065,
		Filipino = 1124,
		Finnish = 1035,
		French_Belgium = 2060,
		French_Cameroon = 11276,
		French_Canada = 3084,
		French_Congo = 9228,
		French_Cote_dIvoire = 12300,
		French_France = 1036,
		French_Luxembourg = 5132,
		French_Mali = 13324,
		French_Monaco = 6156,
		French_Morocco = 14348,
		French_Senegal = 10252,
		French_Switzerland = 4108,
		French_West_Indies = 7180,
		Frisian_Netherlands = 1122,
		Gaelic_Ireland = 2108,
		Gaelic_Scotland = 1084,
		Galician = 1110,
		Georgian = 1079,
		German_Austria = 3079,
		German_Germany = 1031,
		German_Liechtenstein = 5127,
		German_Luxembourg = 4103,
		German_Switzerland = 2055,
		Greek = 1032,
		Guarani_Paraguay = 1140,
		Gujarati = 1095,
		HID = 1279,
		Hebrew = 1037,
		Hindi = 1081,
		Hungarian = 1038,
		Icelandic = 1039,
		Igbo_Nigeria = 1136,
		Indonesian = 1057,
		Italian_Italy = 1040,
		Italian_Switzerland = 2064,
		Japanese = 1041,
		Kannada = 1099,
		Kashmiri = 1120,
		Kazakh = 1087,
		Khmer = 1107,
		Konkani = 1111,
		Korean = 1042,
		Kyrgyz_Cyrillic = 1088,
		Lao = 1108,
		Latin = 1142,
		Latvian = 1062,
		Lithuanian = 1063,
		Malay_Brunei = 2110,
		Malay_Malaysia = 1086,
		Malayalam = 1100,
		Maltese = 1082,
		Manipuri = 1112,
		Maori = 1153,
		Marathi = 1102,
		Mongolian = 2128,
		Mongolian2 = 1104,
		Nepali = 1121,
		Norwegian_Bokml = 1044,
		Norwegian_Nynorsk = 2068,
		Oriya = 1096,
		Polish = 1045,
		Portuguese_Brazil = 1046,
		Portuguese_Portugal = 2070,
		Punjabi = 1094,
		Raeto_Romance = 1047,
		Romanian_Moldova = 2072,
		Romanian_Romania = 1048,
		Russian = 1049,
		Russian_Moldova = 2073,
		Sami_Lappish = 1083,
		Sanskrit = 1103,
		Serbian_Cyrillic = 3098,
		Serbian_Latin = 2074,
		Sesotho_Sutu = 1072,
		Setsuana = 1074,
		Sindhi = 1113,
		// Sinhala = si,
		Slovak = 1051,
		Slovenian = 1060,
		Somali = 1143,
		Sorbian = 1070,
		Spanish_Argentina = 11274,
		Spanish_Bolivia = 16394,
		Spanish_Chile = 13322,
		Spanish_Colombia = 9226,
		Spanish_Costa_Rica = 5130,
		Spanish_Dominican_Republic = 7178,
		Spanish_Ecuador = 12298,
		Spanish_El_Salvador = 17418,
		Spanish_Guatemala = 4106,
		Spanish_Honduras = 18442,
		Spanish_Mexico = 2058,
		Spanish_Nicaragua = 19466,
		Spanish_Panama = 6154,
		Spanish_Paraguay = 15370,
		Spanish_Peru = 10250,
		Spanish_Puerto_Rico = 20490,
		Spanish_Spain_Traditional = 1034,
		Spanish_Uruguay = 14346,
		Spanish_Venezuela = 8202,
		Swahili = 1089,
		Swedish_Finland = 2077,
		Swedish_Sweden = 1053,
		Syriac = 1114,
		Tajik = 1064,
		Tamil = 1097,
		Tatar = 1092,
		Telugu = 1098,
		Thai = 1054,
		Tibetan = 1105,
		Tsonga = 1073,
		Turkish = 1055,
		Turkmen = 1090,
		Ukrainian = 1058,
		Unicode = 0,
		Urdu = 1056,
		Uzbek_Cyrillic = 2115,
		Uzbek_Latin = 1091,
		Venda = 1075,
		Vietnamese = 1066,
		Welsh = 1106,
		Xhosa = 1076,
		Yiddish = 1085,
		Zulu = 1077,
	};

	// class String
	// {
	// 	//map <language_id, PROGMEM const char*>
	// 	std::constexpr_vector<std::pair<LanguageID, const char*>> m_strings;
	// public:
	// 	constexpr String(){}
	// };

	using char_t = wchar_t;

	class StringDescriptorTable
	{
		// std::constexpr_vector<std::constexpr_vector<std::pair<LanguageID, const char*>>*> *m_Strings;
		// std::constexpr_vector<LanguageID> *m_Languages;
		// uint8_t m_numLanguages;
	public:
		using Str = std::pair<uint8_t, const wchar_t*>;
		using Arr = std::constexpr_vector<const Str*>;
		const Arr *m_strings;
		const LanguageID m_language;

		constexpr StringDescriptorTable(
			LanguageID _language, 
			const Arr *_strings
		) : 
			m_strings{_strings}, 
			m_language{_language} {}
			
		constexpr Str stringPgmThis(uint8_t idx) const {
			AVR::pgm_ptr _ptr{&m_strings};
			AVR::pgm_ptr _arr{*_ptr};

			const Arr arr = *_arr;
			if(--idx >= arr.size())
				return {0, nullptr};
			AVR::pgm_ptr buf{arr.begin()};
			return *AVR::pgm_ptr{buf[idx]};
		}
		constexpr Str string(uint8_t idx) const {
			// AVR::pgm_ptr<const Arr*> _ptr{&m_strings};

			AVR::pgm_ptr _arr{m_strings};

			Arr vec = *_arr;
			if(--idx >= vec.size())
				return {0, nullptr};
			AVR::pgm_ptr arr{vec.begin()};
			return *AVR::pgm_ptr{arr[idx]};
		}
	};

	class StringDescriptor : public Descriptor
	{
	public:
		constexpr StringDescriptor() :
			Descriptor(nullptr) {}
	};

} // namespace AVR::USB
