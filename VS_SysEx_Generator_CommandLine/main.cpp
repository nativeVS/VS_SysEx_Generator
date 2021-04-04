#include <iostream>
#include <fstream>
#include <limits>
#include <filesystem>

using byte = std::uint8_t;

constexpr int sysExLength{ 14 };
constexpr int dataStart{ 7 };

constexpr byte defaultSysEx[sysExLength]{ 0xf0, 0x00, 0x21, 0x57, 0x01, 0x02, 0x05, 0x01, 0x01, 0x0b, 0x15, 0x14, 0x4E, 0xf7 };

void ignoreLine() {
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool failedInput() {
	if (std::cin.fail()) {
		std::cin.clear();
		ignoreLine();
		return 1;
	}
	return 0;
}


void printChannelInvalid(byte defaultValue) {
	std::cerr << "Entered Channel Number is Invalid! MIDI Channel will be set to defualt value "
		<< static_cast<int>(defaultValue) << ".\n";
}

byte enterChannel() {
	std::cout << "Enter MIDI Channel: ";
	int midiChannel{};
	std::cin >> midiChannel;
	byte defaultValue{ defaultSysEx[dataStart] };
	if (failedInput() || midiChannel > 16 || midiChannel < 1) {
		printChannelInvalid(defaultValue);
		return defaultValue;
	}
	return static_cast<byte>(midiChannel);
}

void printFaderInvalid(int number, byte defaultValue) {
	std::cerr << "Entered MIDI CC Value is Invalid! MIDI CC of fader " << number
		<< " will be set to default value " << static_cast<int>(defaultValue) << ".\n";
}

byte enterFader(int number) {
	std::cout << "Enter CC Number for fader " << number << ": ";
	int fader{};
	std::cin >> fader;
	byte defaultValue{ defaultSysEx[number + dataStart] };
	if (failedInput() || fader > 127 || fader < 0) {
		printFaderInvalid(number, defaultValue);
		return defaultValue;
	}
	return static_cast<char>(fader);
}

byte checksum(byte arg) {
	static byte sum_s{ 0 };
	sum_s += arg;
	return (0xff - sum_s);
}

int main() {

	byte sysEx[sysExLength]{};
	std::copy(std::begin(defaultSysEx), std::end(defaultSysEx), std::begin(sysEx));

	sysEx[dataStart] = enterChannel();
	sysEx[dataStart + 1] = enterFader(1);
	sysEx[dataStart + 2] = enterFader(2);
	sysEx[dataStart + 3] = enterFader(3);
	sysEx[dataStart + 4] = enterFader(4);

	//calculate checksum from sysEx array
	for (int i{ 1 }; i < (sysExLength - 2); i++) {
		sysEx[sysExLength - 2] = checksum(sysEx[i]);
	}

	std::string outputPath = std::filesystem::current_path().generic_string();
	std::ofstream outputFile{ outputPath + "/FaderAssignment.syx" };
	if (!outputFile) {
		std::cerr << "No ouput File could be generated in parent directory, please make sure to execute application in write enabled directory.\n";
		return 1;
	}

	for (int i{ 0 }; i < sysExLength; i++) {
		outputFile << sysEx[i];
	}

	std::cout << "SysEx file of name FaderAssignment.syx has been generated succesfully in parent directory:\n"
		<< outputPath << "\nPress Enter key to terminate program.\n";

	std::cin.clear();
	ignoreLine();
	std::cin.get();

	return 0;
}