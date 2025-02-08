#include <gtest/gtest.h>
#include "geometry/mesh/Mesh.hpp"
#include <filesystem>
#include <fstream>

class MeshTest : public ::testing::Test
{
protected:
    std::string testObjPath = "test_mesh.obj";
    std::string testSegPath = "test_mesh.seg";
    Mesh *mesh;

    void SetUp() override
    {
        std::ofstream objFile(testObjPath);
        objFile << "v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n";
        objFile.close();

        mesh = new Mesh(testObjPath);
    }

    void TearDown() override
    {
        delete mesh;
        std::filesystem::remove(testObjPath);
        std::filesystem::remove(testSegPath);
    }
};

TEST_F(MeshTest, LoadMeshFromObj)
{
    EXPECT_EQ(mesh->getMeshVertices().size(), 3);
    EXPECT_EQ(mesh->getMeshFaces().size(), 1);
}

TEST_F(MeshTest, CreateSegmentationFromSegFile)
{
    std::ofstream segFile(testSegPath);
    segFile << "0\n";
    segFile.close();

    int clusters = mesh->createSegmentationFromSegFile(testSegPath);
    EXPECT_EQ(clusters, 1);
}

TEST_F(MeshTest, BuildFaceAdjacency)
{
    mesh->buildFaceAdjacency();
    EXPECT_FALSE(mesh->getFaceAdjacency().empty());
}

TEST_F(MeshTest, ExportToObj)
{
    std::string outPath = "exported.obj";
    mesh->exportToObj(outPath, 0);
    EXPECT_TRUE(std::filesystem::exists(outPath));
    std::filesystem::remove(outPath);
}

TEST_F(MeshTest, ExportToGroupedObj)
{
    std::string outPath = "exported_grouped.obj";
    mesh->exportToGroupedObj(outPath);
    EXPECT_TRUE(std::filesystem::exists(outPath));
    std::filesystem::remove(outPath);
}
