#include "c2p.hpp"
#include "cxxopts.hpp"

cxxopts::ParseResult parse_options(int argc, char* argv[]) {
    cxxopts::Options options("c2pconvert", "Convert a C2P image to a PNG image");
    options.add_options()
		("h,help", "Print usage")
		("i,input", "Input C2P file", cxxopts::value<std::string>())
		("o,output", "Output PNG file", cxxopts::value<std::string>());

    options.parse_positional({"input", "output"});
    options.positional_help("<input> <output>").show_positional_help();

    try {
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help(); 
            exit(0);
        }

        if (result.count("input") != 1 || result.count("output") != 1) {
            std::cout << options.help();
            exit(0);
        }
        return result;
    } catch (cxxopts::OptionParseException& exception) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    auto result = parse_options(argc, argv);

    try {
        C2PFile file(result["input"].as<std::string>());
        file.write_png(result["output"].as<std::string>());
    } catch (std::exception &exception) {
        std::cout << "Error: " << exception.what();
        exit(0);
    }    

    return 0;
}
