void print_word_map(const std::unordered_map<std::string, helper> &word_map)
{
    for (const auto &p : word_map)
    {
        const std::string &word = p.first;
        const helper &h = p.second;

        std::cout << "Word: " << word << "\n";

        size_t count = h.doc_ids.size();
        for (size_t i = 0; i < count; i++)
        {
            std::cout << "  DocID: " << h.doc_ids[i] << " Locations: ";

            const auto &locs = h.positions[i];
            for (size_t j = 0; j < locs.size(); j++)
            {
                std::cout << locs[j];
                if (j + 1 != locs.size())
                    std::cout << ", ";
            }
            std::cout << "\n";
        }
    }
}
