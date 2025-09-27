// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOC_NATIONS_H
#define IOC_NATIONS_H

#include <algorithm>
#include <QString>
#include "helpers.h"

namespace fm
{
namespace IocNations
{
	struct Nation { unsigned ref; const char* code; const char* name; };

	static Nation nations[204+1] =
	{
		{0, "Code", "NATION"},
		{1, "AFG", "Afghanistan"},
		{2, "ALB", "Albania"},
		{3, "ALG", "Algeria"},
		{4, "AND", "Andorra"},
		{5, "ANG", "Angola"},
		{6, "ANT", "Antigua and Barbuda"},
		{7, "ARG", "Argentina"},
		{8, "ARM", "Armenia"},
		{9, "ARU", "Aruba"},
		{10, "ASA", "American Samoa"},
		{11, "AUS", "Australia"},
		{12, "AUT", "Austria"},
		{13, "AZE", "Azerbaijan"},
		{14, "BAH", "Bahamas"},
		{15, "BAN", "Bangladesh"},
		{16, "BAR", "Barbados"},
		{17, "BDI", "Burundi"},
		{18, "BEL", "Belgium"},
		{19, "BEN", "Benin"},
		{20, "BER", "Bermuda"},
		{21, "BHU", "Bhutan"},
		{22, "BIH", "Bosnia and Herzegovina"},
		{23, "BIZ", "Belize"},
		{24, "BLR", "Belarus"},
		{25, "BOL", "Bolivia"},
		{26, "BOT", "Botswana"},
		{27, "BRA", "Brazil"},
		{28, "BRN", "Bahrain"},
		{29, "BRU", "Brunei"},
		{30, "BUL", "Bulgaria"},
		{31, "BUR", "Burkina Faso"},
		{32, "CAF", "Central African Republic"},
		{33, "CAM", "Cambodia"},
		{34, "CAN", "Canada"},
		{35, "CAY", "Cayman Islands"},
		{36, "CGO", "Congo"},
		{37, "CHA", "Chad"},
		{38, "CHI", "Chile"},
		{39, "CHN", "China"},
		{40, "CIV", "Côte d'Ivoire"},
		{41, "CMR", "Cameroon"},
		{42, "COD", "DR Congo"},
		{43, "COK", "Cook Islands"},
		{44, "COL", "Colombia"},
		{45, "COM", "Comoros"},
		{46, "CPV", "Cape Verde"},
		{47, "CRC", "Costa Rica"},
		{48, "CRO", "Croatia"},
		{49, "CUB", "Cuba"},
		{50, "CYP", "Cyprus"},
		{51, "CZE", "Czech Republic"},
		{52, "DEN", "Denmark"},
		{53, "DJI", "Djibouti"},
		{54, "DMA", "Dominica"},
		{55, "DOM", "Dominican Republic"},
		{56, "ECU", "Ecuador"},
		{57, "EGY", "Egypt"},
		{58, "ERI", "Eritrea"},
		{59, "ESA", "El Salvador"},
		{60, "ESP", "Spain"},
		{61, "EST", "Estonia"},
		{62, "ETH", "Ethiopia"},
		{63, "FIJ", "Fiji"},
		{64, "FIN", "Finland"},
		{65, "FRA", "France"},
		{66, "FSM", "Micronesia"},
		{67, "GAB", "Gabon"},
		{68, "GAM", "Gambia"},
		{69, "GBR", "Great Britain"},
		{70, "GBS", "Guinea-Bissau"},
		{71, "GEO", "Georgia"},
		{72, "GEQ", "Equatorial Guinea"},
		{73, "GER", "Germany"},
		{74, "GHA", "Ghana"},
		{75, "GRE", "Greece"},
		{76, "GRN", "Grenada"},
		{77, "GUA", "Guatemala"},
		{78, "GUI", "Guinea"},
		{79, "GUM", "Guam"},
		{80, "GUY", "Guyana"},
		{81, "HAI", "Haiti"},
		{82, "HKG", "Hong Kong"},
		{83, "HON", "Honduras"},
		{84, "HUN", "Hungary"},
		{85, "INA", "Indonesia"},
		{86, "IND", "India"},
		{87, "IRI", "Iran"},
		{88, "IRL", "Ireland"},
		{89, "IRQ", "Iraq"},
		{90, "ISL", "Iceland"},
		{91, "ISR", "Israel"},
		{92, "ISV", "Virgin Islands"},
		{93, "ITA", "Italy"},
		{94, "IVB", "British Virgin Islands"},
		{95, "JAM", "Jamaica"},
		{96, "JOR", "Jordan"},
		{97, "JPN", "Japan"},
		{98, "KAZ", "Kazakhstan"},
		{99, "KEN", "Kenya"},
		{100, "KGZ", "Kyrgyzstan"},
		{101, "KIR", "Kiribati"},
		{102, "KOR", "South Korea"},
		{103, "KSA", "Saudi Arabia"},
		{104, "KUW", "Kuwait"},
		{105, "LAO", "Laos"},
		{106, "LAT", "Latvia"},
		{107, "LBA", "Libya"},
		{108, "LBR", "Liberia"},
		{109, "LCA", "Saint Lucia"},
		{110, "LES", "Lesotho"},
		{111, "LIB", "Lebanon"},
		{112, "LIE", "Liechtenstein"},
		{113, "LTU", "Lithuania"},
		{114, "LUX", "Luxembourg"},
		{115, "MAD", "Madagascar"},
		{116, "MAR", "Morocco"},
		{117, "MAS", "Malaysia"},
		{118, "MAW", "Malawi"},
		{119, "MDA", "Moldova"},
		{120, "MDV", "Maldives"},
		{121, "MEX", "Mexico"},
		{122, "MGL", "Mongolia"},
		{123, "MHL", "Marshall Islands"},
		{124, "MKD", "Macedonia"},
		{125, "MLI", "Mali"},
		{126, "MLT", "Malta"},
		{127, "MNE", "Montenegro"},
		{128, "MON", "Monaco"},
		{129, "MOZ", "Mozambique"},
		{130, "MRI", "Mauritius"},
		{131, "MTN", "Mauritania"},
		{132, "MYA", "Myanmar"},
		{133, "NAM", "Namibia"},
		{134, "NCA", "Nicaragua"},
		{135, "NED", "Netherlands"},
		{136, "NEP", "Nepal"},
		{137, "NGR", "Nigeria"},
		{138, "NIG", "Niger"},
		{139, "NOR", "Norway"},
		{140, "NRU", "Nauru"},
		{141, "NZL", "New Zealand"},
		{142, "OMA", "Oman"},
		{143, "PAK", "Pakistan"},
		{144, "PAN", "Panama"},
		{145, "PAR", "Paraguay"},
		{146, "PER", "Peru"},
		{147, "PHI", "Philippines"},
		{148, "PLE", "Palestine"},
		{149, "PLW", "Palau"},
		{150, "PNG", "Papua New Guinea"},
		{151, "POL", "Poland"},
		{152, "POR", "Portugal"},
		{153, "PRK", "North Korea"},
		{154, "PUR", "Puerto Rico"},
		{155, "QAT", "Qatar"},
		{156, "ROU", "Romania"},
		{157, "RSA", "South Africa"},
		{158, "RUS", "Russia"},
		{159, "RWA", "Rwanda"},
		{160, "SAM", "Samoa"},
		{161, "SEN", "Senegal"},
		{162, "SEY", "Seychelles"},
		{163, "SIN", "Singapore"},
		{164, "SKN", "Saint Kitts and Nevis"},
		{165, "SLE", "Sierra Leone"},
		{166, "SLO", "Slovenia"},
		{167, "SMR", "San Marino"},
		{168, "SOL", "Solomon Islands"},
		{169, "SOM", "Somalia"},
		{170, "SRB", "Serbia"},
		{171, "SRI", "Sri Lanka"},
		{172, "STP", "São Tomé and Príncipe"},
		{173, "SUD", "Sudan"},
		{174, "SUI", "Switzerland"},
		{175, "SUR", "Suriname"},
		{176, "SVK", "Slovakia"},
		{177, "SWE", "Sweden"},
		{178, "SWZ", "Swaziland"},
		{179, "SYR", "Syria"},
		{180, "TAN", "Tanzania"},
		{181, "TGA", "Tonga"},
		{182, "THA", "Thailand"},
		{183, "TJK", "Tajikistan"},
		{184, "TKM", "Turkmenistan"},
		{185, "TLS", "Timor-Leste"},
		{186, "TOG", "Togo"},
		{187, "TPE", "Chinese Taipei"},
		{188, "TRI", "Trinidad and Tobago"},
		{189, "TUN", "Tunisia"},
		{190, "TUR", "Turkey"},
		{191, "TUV", "Tuvalu"},
		{192, "UAE", "United Arab Emirates"},
		{193, "UGA", "Uganda"},
		{194, "UKR", "Ukraine"},
		{195, "URU", "Uruguay"},
		{196, "USA", "United States"},
		{197, "UZB", "Uzbekistan"},
		{198, "VAN", "Vanuatu"},
		{199, "VEN", "Venezuela"},
		{200, "VIE", "Vietnam"},
		{201, "VIN", "Saint Vincent and the Grenadines"},
		{202, "YEM", "Yemen"},
		{203, "ZAM", "Zambia"},
		{204, "ZIM", "Zimbabwe"}
	};

	static const char* const GetNameFromCode(QString const& code)
	{
		auto i = std::find_if(&nations[0],
							  &nations[0]+ArrayLength(nations),
							  [=](Nation n){return code == n.code;} );
		if (i == &nations[0]+ArrayLength(nations))
			return nations[0].name;

		return i->name;
	}

	static const char* const GetCodeFromName(QString const& name)
	{
		auto i = std::find_if(&nations[0],
							  &nations[0]+ArrayLength(nations),
							  [=](Nation n){return name == n.name;} );
		if (i == &nations[0]+ArrayLength(nations))
			return nations[0].code;

		return i->code;
	}

	static QStringList GetList()
	{
		QStringList list;
		for (Nation const& n: nations)
		{
			list.push_back(n.name);
		}
		return list;
	}
};

}
#endif // IOC_NATIONS_H
