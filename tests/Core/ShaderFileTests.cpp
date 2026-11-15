#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <system_error>

#include "ProjectLight/Renderer/Shader.h"

namespace ProjectLight
{
    namespace
    {
        class ShaderFileTests : public testing::Test
        {
        protected:
            void SetUp() override {
                const testing::TestInfo* testInfo =
                    testing::UnitTest::GetInstance()->current_test_info();
                const std::string name{testInfo->name()};
                const std::filesystem::path directory{testing::TempDir()};
                m_VertexPath = directory / (name + ".vert");
                m_FragmentPath = directory / (name + ".frag");
                RemoveFiles();
            }

            void TearDown() override {
                RemoveFiles();
            }

            std::filesystem::path m_VertexPath;
            std::filesystem::path m_FragmentPath;

        private:
            void RemoveFiles() const {
                std::error_code error;
                std::filesystem::remove(m_VertexPath, error);
                std::filesystem::remove(m_FragmentPath, error);
            }
        };

        void ExpectLoadFailureContainsPath(const std::filesystem::path& vertexPath,
                                           const std::filesystem::path& fragmentPath,
                                           const std::filesystem::path& failedPath) {
            try {
                static_cast<void>(Shader::FromFiles(vertexPath, fragmentPath));
                FAIL() << "Expected shader file loading to fail";
            } catch (const std::runtime_error& error) {
                EXPECT_NE(std::string{error.what()}.find(failedPath.string()), std::string::npos);
            }
        }
    }

    TEST_F(ShaderFileTests, MissingVertexFileReportsPath) {
        ExpectLoadFailureContainsPath(m_VertexPath, m_FragmentPath, m_VertexPath);
    }

    TEST_F(ShaderFileTests, EmptyVertexFileReportsPath) {
        std::ofstream output{m_VertexPath};
        ASSERT_TRUE(output);
        output.close();

        ExpectLoadFailureContainsPath(m_VertexPath, m_FragmentPath, m_VertexPath);
    }

    TEST_F(ShaderFileTests, MissingFragmentFileReportsPath) {
        std::ofstream output{m_VertexPath};
        ASSERT_TRUE(output);
        output << "vertex source";
        output.close();

        ExpectLoadFailureContainsPath(m_VertexPath, m_FragmentPath, m_FragmentPath);
    }
}
