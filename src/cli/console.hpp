/*
 * console.hpp
 *
 *  Created on: May 5, 2020
 *      Author: hkooper
 *      from https://raw.githubusercontent.com/Svalorzen/cpp-readline/master/src/console.hpp
 */

#ifndef SRC_CLI_CONSOLE_HPP_
#define SRC_CLI_CONSOLE_HPP_


#include <functional>
#include <string>
#include <vector>
#include <memory>
namespace hqn {
namespace cli {
    class console {
        public:
            /**
             * @brief This is the function type that is used to interface with the console class.
             *
             * These are the functions that are going to get called by console
             * when the user types in a message. The vector will hold the
             * command elements, and the function needs to return its result.
             * The result can either be Quit (-1), OK (0), or an arbitrary
             * error (>=1).
             */
            using Arguments = std::vector<std::string>;
            using CommandFunction = std::function<int(const Arguments &)>;

            enum ReturnCode {
                Quit = -1,
                Ok = 0,
                Error = 1 // Or greater!
            };

            /**
             * @brief Basic constructor.
             *
             * The console comes with two predefined commands: "quit" and
             * "exit", which both terminate the console, "help" which prints a
             * list of all registered commands, and "run" which executes script
             * files.
             *
             * These commands can be overridden or unregistered - but remember
             * to leave at least one to quit ;).
             *
             * @param greeting This represents the prompt of the console.
             */
            explicit console(std::string const& greeting);

            /**
             * @brief Basic destructor.
             *
             * Frees the history which is been produced by GNU readline.
             */
            ~console();

            /**
             * @brief This function registers a new command within the console.
             *
             * If the command already existed, it overwrites the previous entry.
             *
             * @param s The name of the command as inserted by the user.
             * @param f The function that will be called once the user writes the command.
             */
            void registerCommand(const std::string & s, CommandFunction f);

            /**
             * @brief This function returns a list with the currently available commands.
             *
             * @return A vector containing all registered commands names.
             */
            std::vector<std::string> getRegisteredCommands() const;

            /**
             * @brief Sets the prompt for this console.
             *
             * @param greeting The new greeting.
             */
            void setGreeting(const std::string & greeting);

            /**
             * @brief Gets the current prompt of this console.
             *
             * @return The currently set greeting.
             */
            std::string getGreeting() const;

            /**
             * @brief This function executes an arbitrary string as if it was inserted via stdin.
             *
             * @param command The command that needs to be executed.
             *
             * @return The result of the operation.
             */
            int executeCommand(const std::string & command);

            /**
             * @brief This function calls an external script and executes all commands inside.
             *
             * This function stops execution as soon as any single command returns something
             * different from 0, be it a quit code or an error code.
             *
             * @param filename The pathname of the script.
             *
             * @return What the last command executed returned.
             */
            int executeFile(const std::string & filename);

            /**
             * @brief This function executes a single command from the user via stdin.
             *
             * @return The result of the operation.
             */
            int readLine();
        private:
            console(const console&) = delete;
            console(console&&) = delete;
            console& operator = (console const&) = delete;
            console& operator = (console&&) = delete;

            struct impl;
            using PImpl = ::std::unique_ptr<impl>;
            PImpl pimpl_;

            /**
             * @brief This function saves the current state so that some other console can make use of the GNU readline facilities.
             */
            void saveState();
            /**
             * @brief This function reserves the use of the GNU readline facilities to the calling console instance.
             */
            void reserveconsole();

            // GNU newline interface to our commands.
            using commandCompleterFunction = char**(const char * text, int start, int end);
            using commandIteratorFunction = char*(const char * text, int state);

            static commandCompleterFunction getCommandCompletions;
            static commandIteratorFunction commandIterator;
    };

}}

#endif /* SRC_CLI_CONSOLE_HPP_ */
