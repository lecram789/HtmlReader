#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <urlmon.h>

namespace fs = std::experimental::filesystem;

//@TODO: XML File oder JSON File als output 
#define START_POS 10000
#define BYTE_BUFFER_SIZE 1024
#define MAX_READ_BYTES 20000000 // Keine Dateien größer als 20MB laden

const char* loadHtmlIntoString(const char* file, std::ifstream& input) {
	input.open(file, std::ifstream::in);
	if (input) {
		input.seekg(0, input.end);
		uint32_t length = input.tellg();
		input.seekg(0, input.beg);

		char* buffer = length <= MAX_READ_BYTES ? new char[length] : nullptr;

		if (buffer)input.read(buffer, length);

		input.close();

		return buffer;
	}
	return nullptr;
}


const char* loadHtmlIntoString(const std::experimental::filesystem::v1::directory_entry& file, std::ifstream& input) {
	input.open(file, std::ifstream::in);
	if (input) {
		input.seekg(0, input.end);
		uint32_t length = input.tellg();
		input.seekg(0, input.beg);

		char* buffer = length <= MAX_READ_BYTES ? new char[length] : nullptr;

		if (buffer)input.read(buffer, length);

		input.close();

		return buffer;
	}
	return nullptr;
}

void replaceString(std::string& str, std::string ser, std::string rep) {
	uint32_t first_pos = 0;
	while (first_pos != std::string::npos) {
		first_pos = str.find(ser, first_pos);
		str.replace(first_pos, ser.length(), rep);
		++first_pos;
	}

}

void retrieveData(std::ifstream& input) {

	char c = '0';
	char output[] = "files/0.html";

	HRESULT hr = URLDownloadToFile(NULL, "http://www.kicker.de/news/fussball/bundesliga/spieltag/1-bundesliga/2017-18/spieltag.html", "AlleSpieltage.html", 0, NULL);

	std::string content(loadHtmlIntoString("AlleSpieltage.html", input));
	std::string url = "http://www.kicker.de";

	uint32_t pos = 0, pos2 = 0;


	while (pos != std::string::npos) {
		pos2 = content.find("ovVrnLink2015", pos);
		pos = content.find(">Live<", pos2);
		if (pos - pos2 > 500)continue;
		if (pos == std::string::npos)break;
		pos2 = pos - 2;
		while (content[--pos2] != '"'){}
		url.append(content.substr(pos2 + 1, pos - pos2 - 2));
		replaceString(url, "livematch", "livetaktischeaufstellung");
		pos2 = pos + 1;
		hr = URLDownloadToFile(NULL, url.c_str(), output, 0, NULL);
		output[6] = ++c;
	}
	 
	
}

void extractLineup(std::string& content, std::ofstream& output) {

	output << "[\n";

	uint32_t start_pos = content.find("class=\"taktischeaufstellung\"", START_POS);
	uint32_t pos_del1 = 0, pos_del2 = 0;
	for (uint32_t i = 0;i < 24 ;++i, start_pos = ++pos_del2) {
		pos_del1 = content.find("src", start_pos) + 5;
		output << (i==0 ? "\t{ \n\t\t\"url\": \"" : ",\n\t{\n\t\t\"url\": \"");
		while (content[pos_del1] != '"') {
			output << content[pos_del1];
			++pos_del1;
		}
		output << "\",\n";

		start_pos = pos_del1;
		while (true) {
			pos_del1 = content.find('>', start_pos);
			pos_del2 = content.find('<', ++pos_del1);

			start_pos = pos_del2;

			if (content[pos_del1] < 58 || pos_del1 == pos_del2)continue;
			output << "\t\t\"name\": \"";
			while (pos_del1 < pos_del2) {
				output << content[pos_del1];
				++pos_del1;
			}
			output << "\"\n\t}";
			break;
		}
		
	}
	output << "\n]\n";
}

int main(int argc, char* argv[]) {
	//Support für mehrere Html-Files fehlt noch 
	
	//Lade alle Seiten runter die "Live" sind
	

	std::ifstream input;
	std::ofstream output("aufstellung.json", std::ofstream::out);

	retrieveData(input);

	

	for (auto & file : fs::recursive_directory_iterator("files")) {
		input.open(file, std::ifstream::in);
		output.app;

		std::string content(std::string(loadHtmlIntoString(file, input)));
		extractLineup(content, output);
		output.close();
	}
	return 0;
}