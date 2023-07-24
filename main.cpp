#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <variant>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"

bool running_service = true;

std::string formatDescription(std::string description)
{
  char next_line = '\n';

  // Goes through the length of the description
  for (int i = 0; i < description.length(); i++)
  {
    // Insert a new line every 100 characters
    if (i % 100 == 0)
    {
      description.insert(i, 1, next_line);
    }
  }

  return description;
}

rapidjson::Document openAndReadJsonFile(const char *json_file_directory)
{

  // Open the file
  std::ifstream file(json_file_directory);

  // Read the entire file into a string
  std::string json((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

  // Create a Document object
  // to hold the JSON data
  rapidjson::Document json_document_object;

  // Parse the JSON data
  json_document_object.Parse(json.c_str());
  // Create a JSON writer with pretty formatting
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

  // Convert the buffer to a string
  std::string jsonStr = buffer.GetString();

  // Write the JSON string to a file
  std::ofstream outputFile(json_file_directory);

  // Check for parse errors
  if (json_document_object.HasParseError())
  {
    switch (json_document_object.GetParseError())
    {
    case rapidjson::kParseErrorDocumentEmpty:
      std::cout << "Error: Document is empty" << std::endl;
      std::cout << "Making new " << json_file_directory << " instead" << std::endl;
      json_document_object.SetArray();

      // Write the JSON array to the buffer
      json_document_object.Accept(writer);

      if (outputFile.is_open())
      {
        outputFile << jsonStr;
        outputFile.close();
        std::cout << "JSON data has been written to " << json_file_directory << std::endl;
      }
      else
      {
        std::cerr << "Error opening file for writing!" << std::endl;
        running_service = false;
      }

      break;
    case rapidjson::kParseErrorDocumentRootNotSingular:
      std::cout << "Error: Document root is not singular"
                << std::endl;
      running_service = false;
      break;
    }
  }
  else if (json_document_object.GetParseError() == rapidjson::kParseErrorNone)
  {
    std::cout << "No error" << std::endl;
  }

  // Close the file
  // fclose(json_file);
  return json_document_object;
}

void displayAllBooks(const char *json_file_directory)
{
  rapidjson::Document json_document_object = openAndReadJsonFile(json_file_directory);

  std::map<std::string, std::variant<std::string, double>> books;
  std::vector<std::map<std::string, std::variant<std::string, double>>> all_books;

  // Iterate over the array of objects
  rapidjson::Value::ConstValueIterator iterate_value;

  for (iterate_value = json_document_object.Begin(); iterate_value != json_document_object.End(); ++iterate_value)
  {
    // Access the data in the object
    books["title"] = iterate_value->GetObject()["title"].GetString();
    books["description"] = iterate_value->GetObject()["description"].GetString();
    books["price"] = iterate_value->GetObject()["price"].GetDouble();
    all_books.push_back({books});
  }

  int lenth_of_array = all_books.size();
  // Go through all the books
  for (int i = 0; i < lenth_of_array; i++)
  {
    // Access the data in the object
    std::cout << std::get<std::string>(all_books[i]["title"]) << std::endl
              << formatDescription(std::get<std::string>(all_books[0]["description"])) << "\n"
              << std::endl
              << std::get<double>(all_books[0]["price"]) << "\n"
              << std::endl;
  }
}

void writeBookToJson(const char *json_file_directory)
{

  // Create a JSON document to hold the JSON data
  rapidjson::Document json_document_object = openAndReadJsonFile(json_file_directory);

  std::string title;
  std::string description;
  double price;

  std::cout << "Enter Title: ";
  std::getline(std::cin, title);

  std::cout << "Enter Description: ";
  std::getline(std::cin, description);

  std::cout << "Enter Price: ";
  std::cin >> price;

  // Add key-value pairs to the JSON map
  rapidjson::Value map(rapidjson::kObjectType);

  map.AddMember("title", rapidjson::Value(title.c_str(), json_document_object.GetAllocator()).Move(), json_document_object.GetAllocator());
  map.AddMember("description", rapidjson::Value(description.c_str(), json_document_object.GetAllocator()).Move(), json_document_object.GetAllocator());
  map.AddMember("price", price, json_document_object.GetAllocator());

  json_document_object.PushBack(map, json_document_object.GetAllocator());

  // Create a JSON writer with pretty formatting
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

  // Write the JSON array to the buffer
  json_document_object.Accept(writer);

  // Convert the buffer to a string
  std::string jsonStr = buffer.GetString();

  // Write the JSON string to a file
  std::ofstream outputFile(json_file_directory);
  if (outputFile.is_open())
  {
    outputFile << jsonStr;
    outputFile.close();
    std::cout << "JSON data has been written to " << json_file_directory << std::endl;
  }
  else
  {
    std::cerr << "Error opening file for writing!" << std::endl;
    running_service = false;
  }
}
int main()
{
  /* This is an excellent idea to upgrade your skills in C++.
  All you require to do is build an application that will let
  its customers add and display books. Furthermore, it will
  help the customers to change their books by implementing modifications
  to them or adding or deleting pages. Create an application
  that will be user-friendly and effortless to access and use.
  It also lets users check and buy their favorite books anytime,
  anywhere with one click. But you have to be very mindful of
  your graphics as it is the major part of your bookshop
  management system application. */

  char currency = '$';
  const char *json_file_directory = "books.json";

  while (running_service)
  {
    std::string answer;

    std::cout << "Would you like to \"stop running\" \"display all books\" \"add a new book\": ";

    std::getline(std::cin, answer);
    if (answer == "stop running")
    {
      running_service = false;
      std::cout << "Done";
    }
    else if (answer == "display all books")
    {
      displayAllBooks(json_file_directory);
    }
    else if (answer == "add a new book")
    {
      writeBookToJson(json_file_directory);
    }
    else
    {
      return 1;
    }
  }

  return 0;
}