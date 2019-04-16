#ifndef JSON_Included
#define JSON_Included

#include <memory>
#include <istream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

/* Type representing a value represented in JSON format. */
class JSON {
public:
    /* Constructs a JSON object from a literal of some sort. */
    explicit JSON(std::nullptr_t value);
    explicit JSON(double value);
    explicit JSON(std::int64_t value);
    explicit JSON(bool value);
    explicit JSON(const std::string& value);
    
    /* Constructs a JSON array from a list of JSON objects. */
    explicit JSON(const std::vector<JSON>& elems);

    /* Constructs a JSON object from a map. */
    explicit JSON(const std::unordered_map<std::string, JSON>& elems);

    /* Parses a piece of text into JSON format. */
    static JSON parse(std::istream& input);
    static JSON parse(const std::string& input);

    /* Enumeration representing what type of object this is. */
    enum class Type {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        NULLPTR_T
    };

    /* Returns the type of this object. */
    Type type() const;

    /* Accessors. All of these functions will throw a JSONException if the underlying type
     * is incorrect.
     */
    double         asDouble()  const;
    std::int64_t   asInteger() const;
    bool           asBoolean() const;
    std::nullptr_t asNull()    const;
    std::string    asString()  const;

    /* Array accessors. Again, these will throw JSONExceptions if the underlying type is
     * incorrect.
     */
    JSON operator[] (std::size_t index) const;

    /* Object accessors. As usual, these throw JSONExceptions if the underlying type is
     * incorrect.
     */
    JSON operator[] (const std::string& field) const;
    bool contains(const std::string& fieldName) const;
    
    /* Generic accessor. If the wrapped JSON object is a number, looks up the element at that index,
     * assuming this is an array. If the wrapped JSON object is a string, looks up the element with
     * that key, assuming this is an object.
     *
     * Be careful calling this function with wrapped doubles; any rounding errors will cause
     * problems!
     */
    JSON operator[] (JSON key) const;

    /* Shared between arrays and objects. */
    std::size_t size() const;

    /* For arrays, iterates over the elements. For objects, iterates over the keys as JSON strings.
     * For all other types, raises an error.
     */
    class const_iterator;
    const_iterator begin()  const;
    const_iterator end()    const;
    const_iterator cbegin() const;
    const_iterator cend()   const;
    
    friend std::ostream& operator<< (std::ostream& out, JSON json);

private:
    friend class BaseJSON;
    std::shared_ptr<class BaseJSON> mImpl;

    JSON(std::shared_ptr<class BaseJSON> impl);
};

class JSON::const_iterator {
public:
    /* Iterator typedefs. */
    typedef std::ptrdiff_t          difference_type;
    typedef const JSON              value_type;
    typedef const JSON*             pointer;
    typedef const JSON&             reference;
    typedef std::input_iterator_tag iterator_category;

    const_iterator();
    
    bool operator== (const_iterator rhs) const;
    bool operator!= (const_iterator rhs) const;
    
    const_iterator& operator++ ();
    const const_iterator operator++ (int);
    
    const JSON& operator* () const;
    const JSON* operator->() const;

private:
    friend class JSONSource;
    std::shared_ptr<class JSONSource> mImpl;
    
    const_iterator(std::shared_ptr<class JSONSource>);
};

/* Exception type thrown when an error occurs. */
class JSONException: public std::logic_error {
public:
    JSONException(const std::string& reason);
};

#endif
