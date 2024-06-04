/*****************************************************************//**
 * \file   Printing.cpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#pragma once

#include <set>
#include <unordered_set>

#include <map>
#include <unordered_map>

#include <vector>
#include <list>
#include <deque>

#include <stack>
#include <queue>

#include <initializer_list>
#include <tuple>

#include <string>
#include <iostream>
#include <type_traits>
#include <ios>

namespace gep
{
    // stream insert prior to printing to change color
    namespace color
    {
        // must be used to change the color back to normal
        const std::string RESET   = "\033[0m";

        const std::string RED     = "\033[31m";
        const std::string GREEN   = "\033[32m";
        const std::string YELLOW  = "\033[33m";
        const std::string BLUE    = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN    = "\033[36m";
        const std::string PEACH   = "\033[38;5;208m";
    };

    // backend implementation
    namespace detail
    {
        // compares the outermost type of a template, the following: vector<int> == vector<double>, evaluates to true
        template <template<typename...> typename Container, typename Type>
        struct is_similar : std::false_type {};

        template <template<typename...> typename Container, typename... Args>
        struct is_similar<Container, Container<Args...>> : std::true_type {};

        // enables if two items are similar, uses is_similar
        template <template<typename...> typename Container, typename... Args>
        using enable_if_similar = typename std::enable_if<is_similar<Container, Args...>::value>::type;



        // returns true if the type has public facing iterator 
        template <typename T, typename = void>
        struct has_iterator : std::false_type {};

        template <typename T>
        struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};



        // checks if the given type has the member function begin and end
        template <typename T, typename = void>
        struct has_begin_end : std::false_type {};

        template <typename T>
        struct has_begin_end<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};



        // checks if the given type has an operator<<
        template<typename T, typename = void>
        struct has_output_operator : std::false_type {};

        template<typename Type>
        struct has_output_operator<Type, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<Type>())>> : std::true_type {};

        // enables if the given type has an operator<<, uses has_output_operator
        template<typename Type>
        using enable_if_outable = typename std::enable_if<has_output_operator<Type>::value>::type;



        // default class for the printer if it is an unprintable type it will crash here
        template<typename Type, typename Enable = void>
        struct Printer
        {
            static std::ostream& Print_DEPRICATED(const Type& item, std::ostream& os = std::cout)
            {
                os << color::RED << "Attempting to print an unsupported Type" << color::RESET;

                return os;
            };
        };

        // printer for operator<< types
        template<typename Type>
        struct Printer<Type, enable_if_outable<Type>>
        {
            static std::ostream& Print_DEPRICATED(const Type& item, std::ostream& os = std::cout)
            {
                os << color::PEACH << item << color::RESET;

                return os;
            };
        };

        // printer for vector type
        template<typename Type>
        struct Printer<Type, enable_if_similar<std::vector, Type>>
        {
            static std::ostream& Print_DEPRICATED(const Type& item, std::ostream& os = std::cout)
            {
                os << color::GREEN << "Vector: {" << color::RESET;
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    Printer<typename Type::value_type>::Print_DEPRICATED(*currentIt, os);

                    // prints a comma and a space after every element except the last
                    if (currentIt != std::prev(item.end(), 1))
                    {
                        os << color::GREEN << ", " << color::RESET;
                    }
                }
                os << color::GREEN << "}" << color::RESET;

                return os;
            };
        };

        // printer for set type
        template<typename Type>
        struct Printer<Type, enable_if_similar<std::set, Type>>
        {
            static std::ostream& Print_DEPRICATED(const Type& item, std::ostream& os = std::cout)
            {
                os << color::GREEN << "Set: {" << color::RESET;
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    Printer<typename Type::value_type>::Print_DEPRICATED(*currentIt, os);

                    // prints a comma and a space after every element except the last
                    if (currentIt != std::prev(item.end(), 1))
                    {
                        os << color::GREEN << ", " << color::RESET;
                    }
                }
                os << color::GREEN << "}" << color::RESET;

                return os;
            };
        };

        template<typename Type, typename Stream>
        Stream& out_color(const Type& item, Stream& os, size_t indent, const std::string& color)
        {
            os << std::string(indent, ' ') << color << item << color::RESET;

            return os;
        }

        template<typename Type, typename Stream>
        Stream& basic_print(const Type& item, Stream& os, size_t indent)
        {
            // evaluates the type of the container ////////////////////////////////////////////////////

            // simples
            constexpr bool isVector = is_similar<std::vector, Type>::value;
            constexpr bool isList   = is_similar<std::list, Type>::value;
            constexpr bool isDeque  = is_similar<std::deque, Type>::value;

            // set
            constexpr bool isOrderSet = is_similar<std::set, Type>::value;
            constexpr bool isUnordSet = is_similar<std::unordered_set, Type>::value;
            constexpr bool isMultiSet = is_similar<std::multiset, Type>::value;

            // map                      
            constexpr bool isOrderMap = is_similar<std::map, Type>::value;
            constexpr bool isUnordMap = is_similar<std::unordered_map, Type>::value;
            constexpr bool isMultiMap = is_similar<std::multimap, Type>::value;

            // stack/queue
            constexpr bool isStack = is_similar<std::stack, Type>::value;
            constexpr bool isQueue = is_similar<std::queue, Type>::value;

            // other
            constexpr bool isPair   = is_similar<std::pair, Type>::value;
            constexpr bool isSimple = has_output_operator<Type>::value;

            // strings
            constexpr bool isString  = std::is_same<std::string, Type>::value;
            constexpr bool isWString = std::is_same<std::wstring, Type>::value;

            // containers
            constexpr bool isIterable        = has_begin_end<Type>::value;



            // gets the name of the container /////////////////////////////////////////////////////////

            // the name of the container to be printed later
            std::string name = "Container";

            // simples
            if      constexpr (isVector) name = "Vector";
            else if constexpr (isList)   name = "List";
            else if constexpr (isList)   name = "Deque";

            // sets
            else if constexpr (isOrderSet) name = "Set";
            else if constexpr (isUnordSet) name = "UnorderedSet";
            else if constexpr (isMultiSet) name = "MultiSet";

            // maps
            else if constexpr (isOrderMap) name = "Map";
            else if constexpr (isUnordMap) name = "UnorderedMap";
            else if constexpr (isMultiMap) name = "MultiMap";

            // other
            else if constexpr (isPair)   name = "Pair";
            else if constexpr (isString) name = "String";

            // stack/queue
            else if constexpr (isQueue) name = "Queue";
            else if constexpr (isStack) name = "Stack";


            // determine how to print each container //////////////////////////////////////////////////

            constexpr size_t indentAmount = 2;

            // simple items
            if constexpr (isSimple)
            {
                out_color(item, os, indent, color::PEACH);
            }

            // iterable data stuctures
            else if constexpr (isIterable)
            {
                // prints the name and the leading squiggly
                out_color(name + ":", os, indent, color::GREEN) << std::endl;
                out_color("{", os, indent, color::GREEN) << std::endl;

                // iterate each element recursively calling print on each element
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    basic_print(*currentIt, os, indent + indentAmount) << std::endl;
                }

                out_color("}", os, indent, color::GREEN);
            }

            // prints stacks and queues differently because the dont have direct member access
            else if constexpr (isQueue || isStack)
            {
                // prints the name and the leading squiggly
                out_color(name + ":", os, indent, color::GREEN) << std::endl;
                out_color("{", os, indent, color::GREEN) << std::endl;

                // create a temporary so the original is not modified
                Type temp = item;

                // pop items from the temp and print them one by one
                while (!temp.empty()) 
                {
                    // prints the elements using the coresponding data structure function
                    if constexpr (isQueue) basic_print(temp.front(), os, indent + indentAmount) << std::endl;
                    else                   basic_print(temp.top(),   os, indent + indentAmount) << std::endl;

                    // removes the item from the temp
                    temp.pop();
                }

                out_color("}", os, indent, color::GREEN);
            }

            // pairs
            else if constexpr (isPair)
            {
                // prints the name and the leading squiggly
                out_color(name + ":", os, indent, color::GREEN) << std::endl;
                out_color("{", os, indent, color::GREEN) << std::endl;

                // print the contents of the pair
                basic_print(item.first, os, indent + indentAmount) << std::endl;
                basic_print(item.second, os, indent + indentAmount) << std::endl;

                out_color("}", os, indent, color::GREEN);
            }

            // invalid items
            else
            {
                out_color("Attempting to print an unsupported Type", os, indent, color::RED);
            }

            return os;
        }

    } // namespace detail

    // prints out containers/types recursively
    template <typename Type>
    std::ostream& Print_DEPRICATED(const Type& item, std::ostream& os = std::cout)
    {
        detail::Printer<Type>::Print_DEPRICATED(item, os);

        return os;
    }

    template<typename Type>
    std::ostream& Print(const Type& item, std::ostream& os = std::cout)
    {
        detail::basic_print(item, os, 0);

        return os;
    }

    //template<typename Type>
    //std::wostream& PrintW(const Type& item, std::wostream& wos = std::wcout)
    //{
    //    detail::basic_print(item, wos, 0);

    //    return wos;
    //}
}
