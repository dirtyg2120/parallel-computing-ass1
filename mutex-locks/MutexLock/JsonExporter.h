#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

class JsonExporter {
    public:
        JsonExporter() = default;

        // Add a key-value pair to the JSON object
        void add(const std::string& key, const std::string& value) {
            data[key] = value;
        }

        // Add a key-value pair where the value is an integer
        void add(const std::string& key, int value) {
            data[key] = value;
        }

        // Add a key-value pair where the value is a double
        void add(const std::string& key, double value) {
            data[key] = value;
        }

        // Add a nested JSON object
        void add(const std::string& key, const JsonExporter& nested) {
            data[key] = nested.data;
        }

        // Add a key-value pair where the value is a double
        void add(const std::string& key, const std::vector<int>& value) {
            data[key] = value;
        }

        // Add a key-value pair where the value is a std::vector<JsonExporter>
        void add(const std::string& key, const std::vector<JsonExporter>& nestedArray) {
            json jsonArray = json::array();
            for (const auto& nested : nestedArray) {
                jsonArray.push_back(nested.data);
            }
            data[key] = jsonArray;
        }

        // Export the JSON object to a string
        std::string exportToString() const {
            return data.dump(4);
        }

        // Export the JSON object to a file
        void exportToFile(const std::string& filename) const {
            std::ofstream outputfile(filename);
            if (outputfile.is_open()) {
                outputfile << data.dump(4);
                outputfile.close();
                std::cout << "Results are exported" << std::endl;
            }
            else {
                std::cerr << "Unable to open file: " << filename << std::endl;
            }
        }

    private:
        json data;
};

