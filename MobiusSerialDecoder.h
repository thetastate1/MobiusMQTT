const char Hyd26HD[] PROGMEM         = "Hydra 26HD";
const char Hyd32HD[] PROGMEM         = "Hydra 32HD";
const char Hyd52HD[] PROGMEM         = "Hydra 52HD";
const char Hyd64HD[] PROGMEM         = "Hydra 64HD";
const char Nero3[] PROGMEM           = "Nero 3";
const char Nero5[] PROGMEM           = "Nero 5";
const char Prme[] PROGMEM            = "Prime";
const char Prme16HD[] PROGMEM        = "Prime16 HD";
const char Prme16HdFuge[] PROGMEM    = "Prime16 HDFuge";
const char Prme16HdFw[] PROGMEM      = "Prime16 HDFw";
const char Prme16HdSol[] PROGMEM     = "Prime16 HDSol";
const char PrmeFuge[] PROGMEM        = "Prime Fuge";
const char PrmeFW[] PROGMEM          = "Prime FW";
const char PrmeHD[] PROGMEM          = "PrimeHD";
const char RadXR15G5P[] PROGMEM      = "Radion XR15 G5Pro";
const char RadXR15wFW[] PROGMEM      = "Radion XR15w FW";
const char RadXR15wFwP[] PROGMEM     = "Radion XR15w FWPro";
const char RadXR15wG3P[] PROGMEM     = "Radion XR15w G3Pro";
const char RadXR15wG4P[] PROGMEM     = "Radion XR15w G4Pro";
const char RadXR15wG5Bl[] PROGMEM    = "Radion XR15w G5 Blue";
const char RadXR15wPtotype[] PROGMEM = "Radion XR15w Proto";
const char RadXR30w[] PROGMEM        = "Radion XR30w";
const char RadXR30wFwP[] PROGMEM     = "Radion XR30w FWPro";
const char RadXR30wG2[] PROGMEM      = "Radion XR30w G2";
const char RadXR30wG3[] PROGMEM      = "Radion XR30w G3";
const char RadXR30wG3P[] PROGMEM     = "Radion XR30w G3Pro";
const char RadXR30wG4[] PROGMEM      = "Radion XR30w G4";
const char RadXR30wG4P[] PROGMEM     = "Radion XR30w G4Pro";
const char RadXR30wG5Bl[] PROGMEM    = "Radion XR30w G5 Blue";
const char RadXR30wG5P[] PROGMEM     = "Radion XR30w G5Pro";
const char RadXR30wP[] PROGMEM       = "Radion XR30wPro";
const char RadXR30wPtotype[] PROGMEM = "Radion XR30w Proto";
const char RSeaMAX13[] PROGMEM       = "RedSea MAX13";
const char RSeaMAX26[] PROGMEM       = "RedSea MAX26";
const char VectL1[] PROGMEM          = "Vectra L1";
const char VectL2[] PROGMEM          = "VectraL2";
const char VectM1[] PROGMEM          = "Vectra M1";
const char VectM2[] PROGMEM          = "Vectra M2";
const char VectS1[] PROGMEM          = "Vectra S1";
const char VectS2[] PROGMEM          = "Vectra S2";
const char VersaVX1[] PROGMEM        = "Versa VX-1";
const char VorTMP10wES[] PROGMEM     = "VorTech MP10w ES";
const char VorTMP10wQD[] PROGMEM     = "VorTech MP10w QD";
const char VorTMP40wES[] PROGMEM     = "VorTech MP40w ES";
const char VorTMP40wG3QD[] PROGMEM   = "VorTech MP40w G3 QD";
const char VorTMP40wQD[] PROGMEM     = "VorTech MP40w QD";
const char VorTMP60wES[] PROGMEM     = "VorTech MP60w ES";
const char VorTMP60wQD[] PROGMEM     = "VorTech MP60w QD";
const char Unknown[] PROGMEM         = "Unknown";
const char reserved[] PROGMEM        = "Reserved";

const char* getEtmModelInfo(short s) {
	if (s == 1) {
		return VorTMP10wES;
	}
	if (s == 21) {
		return VorTMP10wES;
	}
	if (s == 31) {
		return VorTMP10wES;
	}
	if (s == 3) {
		return VorTMP40wES;
	}
	if (s == 4) {
		return VorTMP60wES;
	}
	if (s == 7) {
		return RadXR30w;
	}
	if (s == 8) {
		return RadXR30wG2;
	}
	if (s == 9) {
		return RadXR30wP;
	}
	if (s == 23) {
		return VorTMP40wES;
	}
	if (s == 24) {
		return VorTMP60wES;
	}
	if (s == 33) {
		return VorTMP40wES;
	}
	if (s != 34) {
		switch (s) {
			case 27:
				return RadXR30w;
			case 28:
				return RadXR30wG2;
			case 29:
				return RadXR30wP;
			default:
				switch (s) {
					case 37:
						return RadXR30w;
					case 38:
						return RadXR30wG2;
					case 39:
						return RadXR30wP;
					case 40:
						return VectS1;
					case 41:
						return VectS1;
					case 42:
						return VectS2;
					case 43:
						return VectS2;
					case 44:
						return VectM2;
					case 45:
						return VectM2;
					case 46:
						return VectL2;
					case 47:
						return VectL2;
					default:
						switch (s) {
							case 52:
								return RadXR30wG3;
							case 53:
								return RadXR30wG3;
							case 54:
								return RadXR30wG3;
							case 55:
								return RadXR30wG3P;
							case 56:
								return RadXR30wG3P;
							case 57:
								return RadXR30wG3P;
							case 58:
								return RadXR15wG3P;
							case 59:
								return RadXR15wG3P;
							case 60:
								return RadXR15wG3P;
							case 61:
								return RadXR15wFW;
							case 62:
								return RadXR15wFW;
							case 63:
								return RadXR15wFW;
							case 64:
								return RadXR30wG3;
							case 65:
								return RadXR30wG3;
							case 66:
								return RadXR30wG3;
							case 67:
								return RadXR30wG3P;
							case 68:
								return RadXR30wG3P;
							case 69:
								return RadXR30wG3P;
							case 70:
								return VorTMP10wQD;
							case 71:
								return VorTMP10wQD;
							case 72:
								return VorTMP10wQD;
							case 73:
								return VorTMP10wQD;
							case 74:
								return VorTMP10wQD;
							case 75:
								return VorTMP10wQD;
							case 76:
								return VorTMP40wQD;
							case 77:
								return VorTMP40wQD;
							case 78:
								return VorTMP40wQD;
							case 79:
								return VorTMP60wQD;
							case 80:
								return VorTMP60wQD;
							case 81:
								return VorTMP60wQD;
							case 82:
								return VectM1;
							case 83:
								return VectM1;
							case 84:
								return VectL1;
							case 85:
								return VectL1;
							case 86:
								return RadXR15wG4P;
							case 87:
								return RadXR15wG4P;
							case 88:
								return RadXR15wG4P;
							case 89:
								return RadXR30wG4;
							case 90:
								return RadXR30wG4;
							case 91:
								return RadXR30wG4;
							case 92:
								return RadXR30wG4P;
							case 93:
								return RadXR30wG4P;
							case 94:
								return RadXR30wG4P;
							case 95:
								return RadXR15wFwP;
							case 96:
								return RadXR15wFwP;
							case 97:
								return RadXR30wFwP;
							case 98:
								return RadXR30wFwP;
							default:
								return nullptr;
						}
				}
		}
	}
	return VorTMP60wES;
}

const char* Model(short modelNum) {
  switch (modelNum) {
  case 0:
    return Unknown;
    break;
  case 10:
    return VorTMP10wES;
    break;
  case 11:
    return VorTMP10wQD;
    break;
  case 30:
    return RadXR30w;
    break;
  case 31:
    return RadXR30wG2;
    break;
  case 32:
    return RadXR30wP;
    break;
  case 33:
    return RadXR30wG3;
    break;
  case 34:
    return RadXR30wG3P;
    break;
  case 35:
    return RadXR30wG4;
    break;
  case 36:
    return RadXR30wG4P;
    break;
  case 37:
    return RadXR30wFwP;
    break;
  case 39:
    return RadXR30wPtotype;
    break;
  case 40:
    return VorTMP40wES;
    break;
  case 41:
    return VorTMP40wQD;
    break;
  case 42:
    return VorTMP40wG3QD;
    break;
  case 60:
    return VorTMP60wES;
    break;
  case 61:
    return VorTMP60wQD;
    break;
  case 144:
    return VectM1;
    break;
  case 145:
    return VectL1;
    break;
  case 146:
    return VectS1;
    break;
  case 147:
    return VectS2;
    break;
  case 148:
    return VectM2;
    break;
  case 149:
    return VectL2;
    break;
  case 160:
    return RadXR15wG3P;
    break;
  case 161:
    return RadXR15wFW;
    break;
  case 162:
    return RadXR15wG4P;
    break;
  case 163:
    return RadXR15wFwP;
    break;
  case 175:
    return RadXR15wPtotype;
    break;
  case 176:
    return RadXR15G5P;
    break;
  case 177:
    return RadXR15wG5Bl;
    break;
  case 178:
    return reserved;
    break;
  case 179:
    return reserved;
    break;
  case 180:
    return reserved; // Replace with the actual value
    break; 
  case 192:
    return RadXR30wG5P;
    break;
  case 193:
    return RadXR30wG5Bl;
    break;
  case 194:
    return reserved;
    break;
  case 195:
    return reserved;
    break;
  case 196:
    return reserved;
    break;
  case 256:
    return Nero5;
    break;
  case 257:
    return Nero3;
    break;
  case 258:
    return reserved;
    break;
  case 259:
    return reserved;
    break;
  case 260:
    return reserved;
    break;
  case 261:
    return reserved;
    break;
  case 262:
    return reserved;
    break;
  case 263:
    return reserved;
    break;
  case 264:
    return reserved;
    break;
  case 272:
    return VersaVX1;
    break;
  case 275:
    return reserved;
    break;
  case 288:
    return reserved;
    break;
  case 304:
    return reserved;
    break;
  case 320:
    return Prme;
    break;
  case 321:
    return PrmeHD;
    break;
  case 322:
    return PrmeFW;
    break;
  case 323:
    return Hyd26HD;
    break;
  case 324:
    return Hyd52HD;
    break;
  case 325:
    return RSeaMAX13;
    break;
  case 326:
    return RSeaMAX26;
    break;
  case 333:
    return Prme16HD;
    break;
  case 334:
    return Hyd32HD;
    break;
  case 335:
    return Hyd64HD;
    break;
  case 339:
    return Prme16HdFw;
    break;
  case 340:
    return Prme16HdFuge;
    break;
  case 341:
    return Prme16HdSol;
    break;
  case 342:
    return PrmeFuge;
    break;
  case 344:
    return reserved;
    break;
  case 345:
    return reserved;
    break;
  case 346:
    return reserved;
    break;
  case 347:
    return reserved;
    break;
  case 349:
    return reserved;
    break;
  case 350:
    return reserved;
  default:
    return Unknown;
    break;
  }
};
bool isValidEtmSerial(const char* str) {
  // Check length
  if (strlen(str) != 14) {
    return false;
  }

  // Check for non-alphanumeric characters
  for (int i = 0; i < 14; ++i) {
    if (!isalnum(str[i])) {
      return false;
    }
  }

  // Calculate the sum of digits
  int num = 0;
  for (int i = 0; i < 11; ++i) {
    num += (str[i] - '0') * (i + 1);
  }

  // Extract the last two digits
  int i;
  try {
    i = std::stoi(str + 11);
  } catch (const std::exception& unused) {
    i = 0;
  }

  // Check validity
  return num > 0 && num % 256 == i;
}
bool isValidMobiusSerial(const char* str) {
  // Check length and non-alphanumeric characters
  if (strlen(str) != 14) {
    return false;
  }
//  Serial.println("14 long..");

  for (int i = 0; i < 14; ++i) {
    if (!isalnum(str[i])) {
      return false;
    }
  }
//  Serial.println("is alphanumeric..");

  // Calculate the sum of character values
  int i = 0;
  for (int j = 0; j < 12; ++j) {
//    Serial.print(str[j]);
//    Serial.print(": ");
//    Serial.println(static_cast<unsigned short>(str[j]));

    i += static_cast<unsigned short>(str[j]);
  }

//  Serial.print("Processed i: ");
//  Serial.println(i);

  // Extract the last two digits as hexadecimal
  int num;
  try {
    num = std::stoi(str + 12, nullptr, 16);
  } catch (std::exception& e) {
    num = 0;
  }

//  Serial.print("Processed num: ");
//  Serial.println(num);

//  Serial.print("Processed i % 256: ");
//  Serial.println(i % 256);

  // Check validity
  return num == i % 256;
}

bool isValidSerial(std::string str) {
    return isValidMobiusSerial(str.c_str()) || isValidEtmSerial(str.c_str());
}

const char* parseMobiusSerialModel(std::string str){
  short parsedValue = std::stoi(str.substr(2, 2), nullptr, 36);
  return Model(parsedValue); 
}

const char* parseEtmSerialModel(std::string str) {
  short value = std::atoi(str.substr(0, 2).c_str());
  return getEtmModelInfo(value);
}

const char* getModelName(std::string mySerial){
//  Serial.print("Serial #: ");
//  Serial.print(mySerial.c_str());

  if (isValidSerial(mySerial)) {
    if (isValidMobiusSerial(mySerial.c_str())){
      return parseMobiusSerialModel(mySerial);
    }
    else if (isValidEtmSerial(mySerial.c_str())){
      return parseEtmSerialModel(mySerial);      
    }
    else{
      return "Unknown";
    }
  } else {
    return "Invalid";
  }

}

