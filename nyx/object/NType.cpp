#include "NType.h"
#include <map>
#include <utility>

NType::NType(std::string name) : name(std::move(name)) {}

void NType::addField(const std::string& field) { fields.push_back(field); }
