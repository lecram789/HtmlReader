#include <stdio.h>
#include <fstream>
#include <urlmon.h>

void replaceCharacter(unsigned char* uri, unsigned char s, unsigned char r) {
	while (*uri != '\0') {
		if (*uri == s)*uri = r;
		++uri;
	}
}

void extractLineup(std::string& content, std::ofstream& output) {

	uint32_t start_pos = content.find("class=\"taktischeaufstellung\"", 60000);
	uint32_t pos_del1 = 0, pos_del2 = 0;
	for (int i = 0; i < 180; ++i, start_pos = pos_del2 + 1 ) {
		pos_del1 = content.find('>', start_pos);
		pos_del2 = content.find('<', ++pos_del1);

		if (content[pos_del1] < 58 || pos_del1 == pos_del2)continue;
		while (pos_del1 < pos_del2) {
			output << content[pos_del1];
			++pos_del1;
		}
			output << std::endl;
	}
}

int main(int argc, char* argv[]) {

	//Support für mehrere Html-Files fehlt noch 
	HRESULT hr = URLDownloadToFile(NULL, "http://www.kicker.de/news/fussball/bundesliga/spieltag/1-bundesliga/2017-18/-1/0/spieltag.html", "AlleSpieltage.html", 0, NULL);
	if (argc == 0) {
		
	}

	uint32_t input_files;

	std::ifstream input(*argv ? *argv : "test.html", std::ifstream::in);
	std::ofstream output("test2.txt", std::ofstream::out);

	if (input) {
		input.seekg(0, input.end);
		uint32_t length = input.tellg();
		input.seekg(0, input.beg);

		char* buffer = new char[length];
		input.read(buffer, length);
		std::string content(buffer);

		delete buffer;

		extractLineup(content, output);
		output << "Read " << length << " characters" << std::endl;
	}

	
	return 0;
}