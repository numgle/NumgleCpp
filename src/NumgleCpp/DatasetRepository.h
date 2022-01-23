#pragma once

#include <crow.h>
#include <cstdint>
#include <httplib.h>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

using ConvertTable1D = std::vector<std::string>;
using ConvertTable2D = std::vector<std::vector<std::string>>;

struct ConvertTables {
    ConvertTable1D choTable;
    ConvertTable1D jongTable;
    ConvertTable1D jungTable;
    ConvertTable2D cjTable;
    ConvertTable1D hanTable;
    ConvertTable1D engUpperTable;
    ConvertTable1D engLowerTable;
    ConvertTable1D numberTable;
    ConvertTable1D specialTable;
};

class DatasetRepository {
public:
    [[nodiscard]] virtual const ConvertTables &
    getConvertTables() const noexcept = 0;
    virtual void update() = 0;
};

constexpr static uint32_t UPDATE_INTERVAL = 3600;

class WebDatasetRepository : public DatasetRepository {
public:
    WebDatasetRepository(const std::string &url, const std::string &path)
        : cli(url)
        , path(path) {
        update();
    }

    const ConvertTables &getConvertTables() const noexcept override {
        auto lock = std::unique_lock(mutex);
        return cached;
    }

    void update() override {
        auto lock = std::unique_lock(mutex);
        std::time_t current = std::time(0);
        if (current - lastUpdated > UPDATE_INTERVAL) {
            fetch();
        }
        lastUpdated = current;
    }

private:
    void fetch() {
        auto res = cli.Get("/numgle/dataset/main/src/data.json");
        if (res->status != 200) {
            std::cout << "Error while fetching from online" << std::endl;
            return;
        }

        const auto load1DTable = [&](const crow::json::rvalue &json,
                                     ConvertTable1D &table) {
            table.clear();
            table.resize(json.size());
            for (size_t i = 0; i < json.size(); ++i) {
                table[i] = json[i].s();
            }
        };

        const auto load2DTable = [&](const crow::json::rvalue &json,
                                     ConvertTable2D &table) {
            table.clear();
            table.resize(json.size());
            for (size_t i = 0; i < json.size(); ++i) {
                load1DTable(json[i], table[i]);
            }
        };
        auto json = crow::json::load(res->body);
        load1DTable(json["cho"], cached.choTable);
        load1DTable(json["jong"], cached.jongTable);
        load1DTable(json["jung"], cached.jungTable);
        load2DTable(json["cj"], cached.cjTable);
        load1DTable(json["han"], cached.hanTable);
        // load1DTable(json["eng"], cached.engUpperTable);
        load1DTable(json["number"], cached.numberTable);
        load1DTable(json["special"], cached.specialTable);
    }

    ConvertTables cached;
    std::string path;
    httplib::Client cli;
    std::time_t lastUpdated{ 0 };
    mutable std::mutex mutex;
};
