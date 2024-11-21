#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace riddle
{
  class core;
  class type;

  /**
   * @brief A field.
   */
  class field
  {
  public:
    field(type &tp, std::string &&name) noexcept;

    /**
     * @brief Retrieves the type of the field.
     *
     * This function returns a reference to the type of the field.
     *
     * @return A reference to the type of the field.
     */
    [[nodiscard]] type &get_type() const noexcept { return tp; }

    /**
     * @brief Retrieves the name of the field.
     *
     * This function returns a reference to the name of the field.
     *
     * @return A reference to the name of the field.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }

  private:
    type &tp;
    std::string name;
  };

  class scope
  {
  public:
    scope(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args = {});
    virtual ~scope() = default;

  protected:
    /**
     * @brief Adds a field to the scope.
     *
     * This function takes ownership of the provided field and adds it to the scope.
     *
     * @param field A unique pointer to the field to be added.
     */
    void add_field(std::unique_ptr<field> field);

  private:
    core &cr;
    scope &parent;

  protected:
    std::map<std::string, std::unique_ptr<field>, std::less<>> fields;
  };
} // namespace riddle
