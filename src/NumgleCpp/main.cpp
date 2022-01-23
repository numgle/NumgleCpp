#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <crow.h>
#include <unordered_map>

#include <sstream>
#include "NumgleApp.h"
#include "UTF8Decoder.h"

int main() {
    NumgleApp app(std::make_unique< WebDatasetRepository>("https://raw.githubusercontent.com", "/numgle/dataset/main/src/data.json"));
    crow::SimpleApp api;

    CROW_ROUTE(api, "/<str>")([&](const std::string& str) {
        crow::response response{};
        try {
            std::string uriDecoded = uriDecode(str);
            std::vector<uint32_t> codes = splitUnicodes(uriDecoded);
            response.add_header("Content-Type", "text/plain; charset=utf-8");
            response.body = app.convertToNumgle(codes);
        } catch (std::exception e) {
            response.body = "error";
        }
        return response;
    });

    api.port(8080).multithreaded().run();
    return 0;
}
