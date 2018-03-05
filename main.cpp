#include <stdio.h>
#include <fstream>
//#include <urlmon.h>

#define START_POS 10000

void replaceCharacter(unsigned char* uri, unsigned char s, unsigned char r) { //Ersetzt ein Zeichen s mit einem Zeichen r
	while (*uri != '\0') { //Solange das Ende der Zeichenkette nicht erreicht ist
		if (*uri == s)*uri = r; //Falls s gefunden wird erstze es mit r;
		++uri;//Gehe zum nächsten Zeichen 
	}
}

void extractLineup(std::string& content, std::ofstream& output) {

	uint32_t start_pos = content.find("class=\"taktischeaufstellung\"", START_POS);
	uint32_t pos_del1 = 0, pos_del2 = 0;
	for (uint32_t i = 0;i < 22 ;++i, start_pos = pos_del2 + 1) {
		pos_del1 = content.find("src", start_pos) + 5;
		while (content[pos_del1] != '"') {
			output << content[pos_del1];
			++pos_del1;
		}
		output << "\t";

		start_pos = pos_del1;
		while (true) {
			pos_del1 = content.find('>', start_pos);
			pos_del2 = content.find('<', ++pos_del1);

			start_pos = pos_del2 + 1;

			if (content[pos_del1] < 58 || pos_del1 == pos_del2)continue;
			while (pos_del1 < pos_del2) {
				output << content[pos_del1];
				++pos_del1;
			}
			output << std::endl;
			break;
		}
		
	}
}

int main(int argc, char* argv[]) {

	//Support für mehrere Html-Files fehlt noch 
	//HRESULT hr = URLDownloadToFile(NULL, "http://www.kicker.de/news/fussball/bundesliga/spieltag/1-bundesliga/2017-18/-1/0/spieltag.html", "AlleSpieltage.html", 0, NULL);
	if (argc == 0) {
		
	}

	uint32_t input_files;

	std::ifstream input("test.html", std::ifstream::in);
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