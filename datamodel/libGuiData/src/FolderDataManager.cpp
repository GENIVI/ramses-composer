#include "FolderData/FolderDataManager.h"

Folder::Folder() {

}

void Folder::clear() {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        (*it)->clear();
        delete (*it);
        (*it) = nullptr;
    }
    folderList_.clear();
}

std::string Folder::createDefaultFolder() {
    auto func = [=](std::string str)->bool {
        auto it = folderList_.begin();
        while (it != folderList_.end()) {
            if (str == (*it)->getFolderName()) {
                return false;
            }
            it++;
        }
        return true;
    };

    int index{1};
    std::string folder = "Node" + std::to_string(index);
    while (!func(folder)) {
        index++;
        folder = "Node" + std::to_string(index);
    }
    return folder;
}

std::string Folder::createDefaultCurve() {
    auto func = [=](std::string str)->bool {
        auto it = curveList_.begin();
        while (it != curveList_.end()) {
            if (str == (*it)->curve_) {
                return false;
            }
            it++;
        }
        return true;
    };

    int index{1};
    std::string curve = "Curve" + std::to_string(index);
    while (!func(curve)) {
        index++;
        curve = "Curve" + std::to_string(index);
    }
    return curve;
}

void Folder::setVisible(bool visible) {
    visible_ = visible;
}

bool Folder::isVisible() {
    return visible_;
}

void Folder::setParent(Folder *folder) {
    parent_ = folder;
}

Folder *Folder::parent() {
    return parent_;
}

void Folder::setFolderName(std::string name) {
    folderName_ = name;
}

std::string Folder::getFolderName() {
    return folderName_;
}

bool Folder::hasCurve(std::string curve) {
    for (auto it = curveList_.begin(); it != curveList_.end(); it++) {
        if ((*it)->curve_ == curve) {
            return true;
        }
    }
    return false;
}

void Folder::insertCurve(SCurveProperty *curveProp) {
    curveList_.push_back(curveProp);
}

bool Folder::insertCurve(std::string curve, bool bVisible) {
    for (auto it = curveList_.begin(); it != curveList_.end(); it++) {
        if ((*it)->curve_ == curve) {
            return false;
        }
    }
    SCurveProperty *curveProp = new SCurveProperty(curve);
    curveProp->visible_ = bVisible;
    curveList_.push_back(curveProp);
    return true;
}

SCurveProperty *Folder::takeCurve(std::string curve) {
    for (auto it = curveList_.begin(); it != curveList_.end(); it++) {
        if ((*it)->curve_ == curve) {
            SCurveProperty *curveProp = *it;
            curveList_.erase(it);
            return curveProp;
        }
    }
    return nullptr;
}

bool Folder::deleteCurve(std::string curve) {
    for (auto it = curveList_.begin(); it != curveList_.end(); it++) {
        if ((*it)->curve_ == curve) {
            SCurveProperty *curveProp = *it;
            curveList_.erase(it);
            delete curveProp;
            curveProp = nullptr;
            return true;
        }
    }
    return false;
}

SCurveProperty *Folder::getCurve(std::string curve) {
    for (auto it = curveList_.begin(); it != curveList_.end(); it++) {
        if ((*it)->curve_ == curve) {
            return *it;
        }
    }
    return nullptr;
}

std::list<SCurveProperty *> Folder::getCurveList() {
    return curveList_;
}

bool Folder::hasFolder(std::string folderName) {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        if ((*it)->getFolderName() == folderName) {
            return true;
        }
    }
    return false;
}

void Folder::insertFolder(Folder *folder) {
    folder->setParent(this);
    folderList_.push_back(folder);
}

bool Folder::insertFolder(std::string folderName) {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        if ((*it)->getFolderName() == folderName) {
            return false;
        }
    }
    Folder *folder = new Folder;
    folder->setParent(this);
    folder->setFolderName(folderName);
    folderList_.push_back(folder);
    return true;
}

bool Folder::deleteFolder(std::string folderName) {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        if ((*it)->getFolderName() == folderName) {
            Folder *folder = *it;
            folderList_.erase(it);
            folder->clear();
            delete folder;
            folder = nullptr;
            return true;
        }
    }
    return false;
}

Folder *Folder::takeFolder(std::string folderName) {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        if ((*it)->getFolderName() == folderName) {
            Folder *folder = *it;
            folderList_.erase(it);
            return folder;
        }
    }
    return nullptr;
}

Folder *Folder::getFolder(std::string folderName) {
    for (auto it = folderList_.begin(); it != folderList_.end(); it++) {
        if ((*it)->getFolderName() == folderName) {
            return *it;
        }
    }
    return nullptr;
}

std::list<Folder *> Folder::getFolderList() {
    return folderList_;
}

FolderDataManager::FolderDataManager() {
    rootFolder_ = new Folder;
}

void FolderDataManager::clear() {
    rootFolder_->clear();
}

Folder *FolderDataManager::getRootFolder() {
    return rootFolder_;
}

bool FolderDataManager::isCurve(std::string curveName) {
    auto getFolder = [=](Folder **f, std::string node)->bool {
        Folder *temp = *f;
        temp = temp->getFolder(node);
        if (!temp) {
            return false;
        }
        *f = temp;
        return true;
    };

    QStringList list = QString::fromStdString(curveName).split("|");
    std::string curve = list.takeLast().toStdString();
    Folder *folder = rootFolder_;
    for (const QString &node : list) {
        if (!getFolder(&folder, node.toStdString())) {
            return false;
        }
    }
    SCurveProperty *curveProp = folder->getCurve(curve);
    if (curveProp) {
        return true;
    }
    return false;
}

bool FolderDataManager::folderFromPath(std::string path, Folder **folder) {
    auto getFolder = [=](Folder **f, std::string node)->bool {
        Folder *temp = *f;
        temp = temp->getFolder(node);
        if (!temp) {
            return false;
        }
        *f = temp;
        return true;
    };

    Folder *tempFolder = rootFolder_;
    QStringList list = QString::fromStdString(path).split("|");
    std::string last = list.takeLast().toStdString();
    for (const QString &node : list) {
        if (!getFolder(&tempFolder, node.toStdString())) {
            return false;
        }
    }
    if (tempFolder->hasFolder(last)){
        tempFolder = tempFolder->getFolder(last);
    }
    *folder = tempFolder;
    return true;
}

bool FolderDataManager::curveFromPath(std::string curveName, Folder **folder, SCurveProperty **curveProp) {
    auto getFolder = [=](Folder **f, std::string node)->bool {
        Folder *temp = *f;
        temp = temp->getFolder(node);
        if (!temp) {
            return false;
        }
        *f = temp;
        return true;
    };

    Folder *tempFolder = rootFolder_;
    QStringList list = QString::fromStdString(curveName).split("|");
    std::string last = list.takeLast().toStdString();
    for (const QString &node : list) {
        if (!getFolder(&tempFolder, node.toStdString())) {
            return false;
        }
    }
    SCurveProperty *tempCurve{*curveProp};
    if (tempFolder->hasCurve(last)) {
        tempCurve = tempFolder->getCurve(last);
    }
    *folder = tempFolder;
    *curveProp = tempCurve;
    return true;
}

bool FolderDataManager::pathFromCurve(std::string curve, Folder *folder, std::string &path) {
    if (!folder) {
        return false;
    }
    if (!folder->getFolderName().empty()) {
        path = folder->getFolderName() + "|" + curve;
        while (folder->parent()) {
            folder = folder->parent();
            if (!folder->getFolderName().empty()) {
                std::string str = folder->getFolderName() + "|";
                path = str + path;
            }
        }
    } else {
        path = curve;
    }
    return true;
}



