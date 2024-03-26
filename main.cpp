#include <iostream>
#include <string>
#include <chrono>
#include <utility>
#include <vector>
#include <random.hpp>

using Random = effolkronium::random_static;
using namespace std::chrono;

enum class Sharing {
    PUBLIC, PRIVATE
};

class Utils {
public:
    static std::string generateId() {
        const std::string charset = "0123456789abcdef";
        std::string generatedId;
        for (int i = 0; i < 24; i++) {
            int randIndex = Random::get(0, 15);
            generatedId.push_back(charset[randIndex]);
        }
        return generatedId;
    }

//    static std::string make_salt() {
//        /// important este ca salt-ul să fie unic, nu contează că nu este aleatoriu
//        /// pentru fiecare user, salt-ul se stochează ca text clar, lângă parola hashed
//        static uint64_t nr = 1u;
//        std::string salt;
//        auto bytes = static_cast<char *>(static_cast<void *>(&nr));
//        for (unsigned i = 0; i < 16; i++) {
//            salt += bytes[i % 8];
//        }
//        ++nr;
//        return salt;
//    }
//
//    static std::string hash_password(const std::string &plain, const std::string &salt) {
//        return digestpp::blake2b(512).set_salt(salt).absorb(plain).hexdigest();
//    }

    // GENERATED
    static std::string formatDuration(int durationInSeconds) {
        seconds duration(durationInSeconds);

        auto hrsCount = duration_cast<hours>(duration);
        duration -= hrsCount;
        auto minsCount = duration_cast<minutes>(duration);
        duration -= minsCount;
        auto secsCount = duration_cast<seconds>(duration);

        std::stringstream ss;

        if (hrsCount.count() > 0) {
            ss << std::setw(2) << std::setfill('0') << hrsCount.count() << ":";
        }

        ss << std::setw(2) << std::setfill('0') << minsCount.count() << ":"
           << std::setw(2) << std::setfill('0') << secsCount.count();

        return ss.str();
    }
};

// Forward declaration
class User;

class Track {
private:
    std::string trackId;
    std::string title;
    int duration;
    Sharing sharing;
    const User *user;
    std::chrono::year_month_day releaseDate{};

public:
    Track() = delete;

    explicit Track(std::string title_, const User *user_, int duration_ = 0)
            : trackId(Utils::generateId()), title(std::move(title_)), duration(duration_), sharing(Sharing::PUBLIC),
              user(user_) {

        const std::chrono::time_point now{std::chrono::system_clock::now()};
        const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
        releaseDate = std::chrono::year_month_day{ymd};
        // 2024y/March/26
    }

    Track(const Track &other) : trackId(Utils::generateId()), title(other.title), duration(other.duration),
                                sharing(other.sharing), user(other.user), releaseDate(other.releaseDate) {
    }

    Track &operator=(const Track &other) {
        if (this != &other) {
            trackId = Utils::generateId();
            title = other.title;
            duration = other.duration;
            sharing = other.sharing;
            user = other.user;
            releaseDate = other.releaseDate;
        }
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const Track &track) {
        os << "\tTrack ID: " << track.trackId << std::endl;
        os << "\tOwner: " << track.getTrackOwner() << std::endl;
        os << "\tTitle: " << track.title << std::endl;
        os << "\tSharing: " << (track.sharing == Sharing::PUBLIC ? "PUBLIC" : "PRIVATE") << std::endl;
        os << "\tDuration " << Utils::formatDuration(track.duration) << std::endl;
        os << std::endl;
        return os;
    }

    ~Track() = default;

    [[nodiscard]] std::string getTrackOwner() const;

    [[nodiscard]] int getDuration() const {
        return duration;
    }

    [[nodiscard]] std::string getTitle() const {
        return title;
    }

    void setSharing(Sharing sharing_) {
        sharing = sharing_;
    }
};

class Playlist {
private:
    std::string playlistId;
    std::string title;
    std::string description;
    std::vector<Track *> tracks;
    int duration;

public:
    explicit Playlist(std::string title_, std::string description_) : playlistId(Utils::generateId()),
                                                                      title(std::move(title_)),
                                                                      description(std::move(description_)),
                                                                      duration(0) {}

    void addTrack(Track *track) {
        tracks.push_back(track);
        duration += track->getDuration();
    }

    friend std::ostream &operator<<(std::ostream &os, const Playlist &playlist) {
        os << "Playlist ID: " << playlist.playlistId << std::endl;
        os << "Title: " << playlist.title << std::endl;
        os << (!playlist.description.empty() ? "Description: " + playlist.description + "\n" : "");
        os << "Duration: " << Utils::formatDuration(playlist.duration) << std::endl;
        os << "Tracks:\n";
        for (const auto &track: playlist.tracks) {
            os << *track;
        }
        os << std::endl;
        return os;
    }

    ~Playlist() {
        for (auto track: tracks) {
            delete track;
        }
    }
};

class User {
private:
    std::string userId;
    std::string username;
    std::string salt;
    std::string hashedPassword;
    int playlistCount;
    std::vector<Playlist> userPlaylists;

public:
    User() = delete;

    explicit User(std::string username_) : userId(Utils::generateId()),
                                           username(std::move(username_)),
                                           playlistCount(0) {
    }

    void addPlaylist(const Playlist &playlist) {
        userPlaylists.push_back(playlist);
        playlistCount++;
    }

    ~User() = default;


    friend std::ostream &operator<<(std::ostream &os, const User &user) {
        os << "User ID: " << user.userId << std::endl;
        os << "Username: " << user.username << std::endl;
        os << "Password: " << user.hashedPassword << std::endl;
        os << "PlaylistCount: " << user.playlistCount << std::endl;
        os << "Playlists:\n";
        for (const auto &playlist: user.userPlaylists) {
            os << playlist;
        }
        os << std::endl;

        return os;
    }

    [[nodiscard]] const std::string &getUsername() const {
        return username;
    }
};


std::string Track::getTrackOwner() const {
    return user->getUsername();
}


int main() {
    User user1("Qubicon");
    User user2("Travis Scott");

    Playlist user1Playlist("Wave Tracks", "Go with the wave!");
    Track track("Sample Track", &user1, 210);
    std::cout << track.getTitle() << " " << track.getTrackOwner() << std::endl;

    std::cout << "<------------------------------->\n";

    Track track2("One of Us", &user2, 157);
    track2.setSharing(Sharing::PRIVATE);

    std::cout << user1;
    std::cout << "<------------------------------->\n";
    std::cout << user1Playlist;
    std::cout << "<------------------------------->\n";

    user1Playlist.addTrack(&track);
    user1Playlist.addTrack(&track2);

    user1.addPlaylist(user1Playlist);

    std::cout << user1;

    std::cout << "<------------------------------->\n";

}