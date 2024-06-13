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
        // note:
        //  std::disjunction is used for 'or' statements

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// compares the outermost type of a template, the following: vector<int> == vector<double>, evaluates to true
        template <template<typename...> typename Container, typename Type>
        struct is_similar : std::false_type {};

        template <template<typename...> typename Container, typename... Args>
        struct is_similar<Container, Container<Args...>> : std::true_type {};

        // enables if two items are similar, uses is_similar
        template <template<typename...> typename Container, typename... Args>
        using enable_if_similar = typename std::enable_if<is_similar<Container, Args...>::value>::type;



        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// same as is similar but takes multiple comparison types. works as an 'or' ////////////////////////
        template <typename T, template <typename...> class... Templates>
        struct is_similar_multi: std::false_type {};

        template <typename T, template <typename...> class First, template <typename...> class... Rest>
        struct is_similar_multi<T, First, Rest...> : std::disjunction<
            is_similar<First, T>, 
            is_similar_multi<T, Rest...>
        > {};

        // enables if similar to one of multiple other types
        template <typename T, template <typename...> class First, template <typename...> class... Rest>
        using enable_if_similar_multi = typename std::enable_if<is_similar_multi<T, First, Rest...>::value>::type;



        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// returns true if the type has public facing iterator  ////////////////////////////////////////////
        template <typename T, typename = void>
        struct has_iterator : std::false_type {};

        template <typename T>
        struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

        // enables if the given type has an operator<<, uses has_output_operator
        template<typename Type>
        using enable_if_has_iterator = typename std::enable_if_t<has_iterator<Type>::value>;



        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// used because std strings are outable and iterable, causing conflicts
        template<typename T>
        struct is_std_string : std::false_type {};

        template<>
        struct is_std_string<std::string> : std::true_type {};

        template<typename T>
        using enable_if_std_string = std::enable_if_t<is_std_string<T>::value>;

        template<typename T>
        using enable_if_not_std_string = std::enable_if_t<!is_std_string<T>::value>;


        // useful because strings are iterable, but usually you dont want to iterate through strings
        template<typename T>
        using enable_if_iterable_and_not_std_string = std::enable_if_t<has_iterator<T>::value && !is_std_string<T>::value>;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// checks if the given type has the member functions begin and end /////////////////////////////////
        template <typename T, typename = void>
        struct has_begin_end : std::false_type {};

        template <typename T>
        struct has_begin_end<T, std::void_t<
            decltype(std::declval<T>().begin()), 
            decltype(std::declval<T>().end())
        >> : std::true_type {};



        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// checks if the given type has an operator<< //////////////////////////////////////////////////////
        template<typename T, typename = void>
        struct has_output_operator : std::false_type {};

        template<typename Type>
        struct has_output_operator<Type, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<Type>())>> : std::true_type {};

        // enables if the given type has an operator<<, uses has_output_operator
        template<typename Type>
        using enable_if_outable = std::enable_if_t<has_output_operator<Type>::value>;

        template<typename T>
        using enable_if_iterable_and_not_outable = std::enable_if_t<has_iterator<T>::value && !has_output_operator<T>::value>;



        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// checks if the passed type is a template

        template <typename T>
        struct is_template : std::false_type {};

        // Partial specialization for template instances
        template <template <typename...> class Template, typename... Args>
        struct is_template<Template<Args...>> : std::true_type {};

        // Enable if T is a template instantiation
        template <typename T>
        using enable_if_template = std::enable_if_t<is_template<T>::value>;

        // Enable if T is not a template instantiation
        template <typename T>
        using enable_if_not_template = std::enable_if_t<!is_template<T>::value>;


        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// prints a type to the given stream with a given color and indent amount
        template<typename Type, typename Stream>
        Stream& out_color(const Type& item, Stream& os, size_t indent, const std::string& color)
        {
            os << std::string(indent, ' ') << color << item << color::RESET;

            return os;
        }

        // forward declaration of building and running the printer so there can be 2 step recursion
        template<typename Type>
        std::ostream& build_and_run_printer(std::ostream& os, size_t indent, const Type& item);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// default class for the printer if it is an unprintable type it will crash here 
        template<typename Type, typename Enable = void>
        struct Printer
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                return out_color("Attempting to print an unsupported Type", os, indent, color::RED);
            };
        };

        // printer for operator<< types
        template<typename Type>
        struct Printer<Type, enable_if_outable<Type>>
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                return out_color(item, os, indent, color::PEACH);
            };
        };

        // printer for iterable objects type
        template<typename Type>
        struct Printer<Type, enable_if_iterable_and_not_outable<Type>>
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                // prints the name and the leading squiggly
                out_color("{", os, indent, color::GREEN) << std::endl;

                // iterate each element recursively calling print on each element
                for (auto currentIt = item.begin(); currentIt != item.end(); currentIt++)
                {
                    build_and_run_printer(os, indent + 2, *currentIt) << std::endl;
                }

                out_color("}", os, indent, color::GREEN);

                return os;
            };
        };

        // printer for queue type
        template<typename Type>
        struct Printer<Type, enable_if_similar<std::queue, Type>>
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                out_color("{", os, indent, color::GREEN) << std::endl;

                // create a temporary so the original is not modified
                Type temp = item;

                // pop items from the temp and print them one by one
                while (!temp.empty())
                {
                    // prints the elements using the coresponding data structure function
                    build_and_run_printer(os, indent + 2, temp.front()) << std::endl;

                    // removes the item from the temp
                    temp.pop();
                }

                out_color("}", os, indent, color::GREEN);

                return os;
            };
        };

        // printer for stack type
        template<typename Type>
        struct Printer<Type, enable_if_similar<std::stack, Type>>
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                out_color("{", os, indent, color::GREEN) << std::endl;

                // create a temporary so the original is not modified
                Type temp = item;

                // pop items from the temp and print them one by one
                while (!temp.empty())
                {
                    // prints the elements using the coresponding data structure function
                    build_and_run_printer(os, indent + 2, temp.top()) << std::endl;

                    // removes the item from the temp
                    temp.pop();
                }

                out_color("}", os, indent, color::GREEN);

                return os;
            };
        };

        // printer for pair type
        template<typename Type>
        struct Printer<Type, enable_if_similar<std::pair, Type>>
        {
            static std::ostream& basic_print(std::ostream& os, size_t indent, const Type& item)
            {
                out_color("{", os, indent, color::GREEN) << std::endl;

                // print the contents of the pair
                build_and_run_printer(os, indent + 2, item.first) << std::endl;
                build_and_run_printer(os, indent + 2, item.second) << std::endl;

                out_color("}", os, indent, color::GREEN);

                return os;
            };
        };

        template<typename Type>
        std::ostream& build_and_run_printer(std::ostream& os, size_t indent, const Type& item)
        {
            return Printer<Type>::basic_print(os, indent, item);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // prints individual items to a stream
        template<typename Stream, typename Type>
        Stream& basic_print_1(Stream& os, size_t indent, const Type& item)
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
                    basic_print(os, indent + indentAmount, *currentIt) << std::endl;
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
                    if constexpr (isQueue) basic_print(os, indent + indentAmount, temp.front()) << std::endl;
                    else                   basic_print(os, indent + indentAmount, temp.top()) << std::endl;

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
                basic_print(os, indent + indentAmount, item.first) << std::endl;
                basic_print(os, indent + indentAmount, item.second) << std::endl;

                out_color("}", os, indent, color::GREEN);
            }

            // invalid items
            else
            {
                out_color("Attempting to print an unsupported Type", os, indent, color::RED);
            }

            return os;
        }

        // recursion base case
        template<typename Stream>
        Stream& basic_print_args(Stream& os, size_t indent)
        {
            (void)indent;

            return os;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /// prints mutiple arguments to a stream
        template<typename Stream, typename First, typename... Rest>
        Stream& basic_print_args(Stream& os, size_t indent, First&& first, Rest&&... rest)
        {
            //basic_print_1(os, indent, std::forward<First>(first));

            build_and_run_printer(os, indent, std::forward<First>(first));

            basic_print_args(os, indent, std::forward<Rest>(rest)...);

            return os;
        }

    } // namespace detail

    template<typename... Args>
    std::ostream& print(std::ostream& os, Args&&... items)
    {
        detail::basic_print_args(os, 0, std::forward<Args>(items)...);

        return os;
    }

    template<typename... Args>
    std::ostream& print(Args&&... items)
    {
        detail::basic_print_args(std::cout, 0, std::forward<Args>(items)...);

        return std::cout;
    }

    template<typename... Args>
    std::ostream& println(std::ostream& os, Args&&... items)
    {
        detail::basic_print_args(os, 0, std::forward<Args>(items)...) << std::endl;

        return os;
    }

    template<typename... Args>
    std::ostream& println(Args&&... items)
    {
        detail::basic_print_args(std::cout, 0, std::forward<Args>(items)...) << std::endl;

        return std::cout;
    }


    //template<typename Type>
    //std::wostream& PrintW(const Type& item, std::wostream& wos = std::wcout)
    //{
    //    detail::basic_print(item, wos, 0);

    //    return wos;
    //}
}
