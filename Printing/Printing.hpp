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

#include <string>
#include <iostream>
#include <type_traits>

namespace gep
{
    namespace detail
    {
        // evaluates to this for everything that is not a container
        template <template<typename...> typename Container, typename Type>
        struct is_container : std::false_type {};

        // checks if the second type and first type match
        template <template<typename...> typename Container, typename... Args>
        struct is_container<Container, Container<Args...>> : std::true_type {};

        // enables a template if the passed type is a container of a specific type
        template <template<typename...> typename Container, typename... Args>
        using enable_if_container = typename std::enable_if<is_container<Container, Args...>::value>::type;

        // if the passed in type does not have a operator<< it will derive from false type
        template<typename T, typename = void>
        struct has_output_operator : std::false_type {};

        // if the passed in type does have a operator<< it will derive from true type
        template<typename Type>
        struct has_output_operator<Type, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<Type>())>> : std::true_type {};

        template<typename Type>
        using enable_if_outable = typename std::enable_if<has_output_operator<Type>::value>::type;

        // stream insert prior to printing to change color
        namespace Color
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

        // default class for the printer if it is an unprintable type it will crash here
        template<typename Type, typename Enable = void>
        struct Printer
        {
            static std::ostream& Print(std::ostream& os, const Type& item)
            {
                os << Color::RED << "Attempting to print an unsupported Type" << Color::RESET;

                return os;
            };
        };

        // printer for operator<< types
        template<typename Type>
        struct Printer<Type, enable_if_outable<Type>>
        {
            static std::ostream& Print(std::ostream& os, const Type& item)
            {
                os << Color::PEACH << item << Color::RESET;

                return os;
            };
        };

        // printer for vector type
        template<typename Type>
        struct Printer<Type, enable_if_container<std::vector, Type>>
        {
            static std::ostream& Print(std::ostream& os, const Type& item)
            {
                os << Color::GREEN << "Vector: {" << Color::RESET;
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    Printer<typename Type::value_type>::Print(os, *currentIt);

                    // prints a comma and a space after every element except the last
                    if (currentIt != std::prev(item.end(), 1))
                    {
                        os << Color::GREEN << ", " << Color::RESET;
                    }
                }
                os << Color::GREEN << "}" << Color::RESET;

                return os;
            };
        };

        // printer for set type
        template<typename Type>
        struct Printer<Type, enable_if_container<std::set, Type>>
        {
            static std::ostream& Print(std::ostream& os, const Type& item)
            {
                os << Color::GREEN << "Set: {" << Color::RESET;
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    Printer<typename Type::value_type>::Print(os, *currentIt);

                    // prints a comma and a space after every element except the last
                    if (currentIt != std::prev(item.end(), 1))
                    {
                        os << Color::GREEN << ", " << Color::RESET;
                    }
                }
                os << Color::GREEN << "}" << Color::RESET;

                return os;
            };
        };


    } // namespace detail

    // prints out containers/types recursively
    template <typename Type>
    std::ostream& Print(std::ostream& os, const Type& item)
    {
        detail::Printer<Type>::Print(os, item);

        return os;
    }

    template<typename Type>
    std::ostream& Print_depricated(std::ostream& os, const Type& item)
    {
        // evaluates the type of the container ////////////////////////////////////////////////////

        // simples
        constexpr bool isVector   = detail::is_container<std::vector,        Type>::value;
        constexpr bool isList     = detail::is_container<std::list,          Type>::value;
        constexpr bool isSimple   = detail::has_output_operator<             Type>::value;

        // set
        constexpr bool isOrderSet = detail::is_container<std::set,           Type>::value;
        constexpr bool isUnordSet = detail::is_container<std::unordered_set, Type>::value;
        constexpr bool isMultiSet = detail::is_container<std::multiset,      Type>::value;
                                    
        // map                      
        constexpr bool isOrderMap = detail::is_container<std::map,           Type>::value;
        constexpr bool isUnordMap = detail::is_container<std::unordered_map, Type>::value;
        constexpr bool isMultiMap = detail::is_container<std::multimap,      Type>::value;


        // gets the name of the container /////////////////////////////////////////////////////////

        // the name of the container to be printed later
        std::string name;

        // simples
             if constexpr (isVector) name = "Vector";
        else if constexpr (isList)   name = "List";

        // sets
        else if constexpr (isOrderSet) name = "Set";
        else if constexpr (isUnordSet) name = "UnorderedSet";
        else if constexpr (isMultiSet) name = "MultiSet";

        // maps
        else if constexpr (isOrderMap) name = "Map";
        else if constexpr (isUnordMap) name = "UnorderedMap";
        else if constexpr (isMultiMap) name = "MultiMap";


        // determine how to print each container //////////////////////////////////////////////////

        if constexpr (isVector)
        {
            os << detail::Color::GREEN << name << ": {" << detail::Color::RESET;
            for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
            {
                Print(os, *currentIt);

                // prints a comma and a space after every element except the last
                if (currentIt != std::prev(item.end(), 1))
                {
                    os << detail::Color::GREEN << ", " << detail::Color::RESET;
                }
            }
            os << detail::Color::GREEN << "}" << detail::Color::RESET;
        }
        else if constexpr (isOrderSet || isUnordSet)
        {
            os << detail::Color::GREEN << name << ": {" << detail::Color::RESET;
            for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
            {
                Print(os, *currentIt);

                // prints a comma and a space after every element except the last
                if (currentIt != std::prev(item.end(), 1))
                {
                    os << detail::Color::GREEN << ", " << detail::Color::RESET;
                }
            }
            os << detail::Color::GREEN << "}" << detail::Color::RESET;
        }
        else if constexpr (isSimple)
        {
            os << detail::Color::PEACH << item << detail::Color::RESET;
        }
        else
        {
            os << detail::Color::RED << "Attempting to print an unsupported Type" << detail::Color::RESET;
        }

        return os;
    }
}
