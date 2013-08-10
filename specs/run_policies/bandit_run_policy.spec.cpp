#include <specs/specs.h>

go_bandit([](){
  using namespace bandit::detail;

  describe("bandit run policy", [&](){
    std::unique_ptr<contextstack_t> contextstack;
    std::unique_ptr<context> global_context;
    std::string only_pattern;
    std::string skip_pattern;

    before_each([&](){
      contextstack = std::unique_ptr<contextstack_t>(new contextstack_t());
      bool hard_skip = false;
      global_context = std::unique_ptr<context>(new bandit_context("", hard_skip));
      contextstack->push_back(global_context.get());
    });

    describe("neither skip nor only specified", [&](){
    
      before_each([&](){
        only_pattern = "";
        skip_pattern = "";
      });

      it("always says run", [&](){
        bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
        AssertThat(policy.should_run("it name", *contextstack), IsTrue());
      });

      describe("has context marked with 'hard_skip' in stack", [&](){
        std::unique_ptr<context> hard_skip_context;
      
        before_each([&](){
          bool hard_skip = true;
          hard_skip_context = std::unique_ptr<context>(new bandit_context("always ignore", hard_skip));
          contextstack->push_back(hard_skip_context.get());
        }); 

        it("never runs", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsFalse());
          AssertThat(policy.should_run("it name matches 'skip'", *contextstack), IsFalse());
          AssertThat(policy.should_run("it name matches 'only'", *contextstack), IsFalse());
        });
      
      });
    
    });
  
    describe("'skip' specified, 'only' unspecified", [&](){
    
      before_each([&](){
        only_pattern = "";
        skip_pattern = "skip";
      });

      describe("current context matches 'skip'", [&](){
        std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context matches 'skip'", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("never runs", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsFalse());
        });
      
      });

      describe("current context doesn't match 'skip'", [&](){
          std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context doesn't match", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("runs if spec's name doesn't match", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsTrue());
        });

        it("doesn't run if spec's name matches", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name matching 'skip'", *contextstack), IsFalse());
        });
      
      });
      
    });

    describe("'only' specified, 'skip' unspecified", [&](){
    
      before_each([&](){
        only_pattern = "only";
        skip_pattern = "";
      });

      describe("current context matches 'only'", [&](){
        std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context matches 'only'", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("always runs", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsTrue());
        });
      
      });

      describe("current context doesn't match 'only'", [&](){
          std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context doesn't match", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("doesn't run if spec's name doesn't match", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsFalse());
        });

        it("runs if spec's name matches", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name matching 'only'", *contextstack), IsTrue());
        });
      
      });

    });

    describe("'skip' specified, 'only' specified", [&](){
    
      before_each([&](){
        only_pattern = "only";
        skip_pattern = "skip";
      });

      describe("current context matches 'skip'", [&](){
        std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context matches 'skip'", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("doesn't run if 'it' doesn't match 'only'", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsFalse());
        });

        it("runs if 'it' matches 'only'", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it matches 'only'", *contextstack), IsTrue());
        });
      
      });

      describe("current context 'only'", [&](){
          std::unique_ptr<context> current_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context matches 'only'", hard_skip));
          contextstack->push_back(current_context.get());
        });

        it("runs if spec's name doesn't match anything", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsTrue());
        });

        it("doesn't run if spec's name matches 'skip'", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name matching 'skip'", *contextstack), IsFalse());
        });
      
      });

      describe("has both 'only' and 'skip' in context stack", [&](){
          std::unique_ptr<context> current_context;
          std::unique_ptr<context> parent_context;
      
        before_each([&](){
          bool hard_skip = false;
          current_context = std::unique_ptr<context>(new bandit_context("context matches 'only'", hard_skip));
          parent_context = std::unique_ptr<context>(new bandit_context("context matches 'skip'", hard_skip));
          contextstack->push_back(parent_context.get());
          contextstack->push_back(current_context.get());
        });
        
        it("runs if spec's name doesn't match anything", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name", *contextstack), IsTrue());
        });

        it("doesn't run if spec's name matches 'skip'", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name matching 'skip'", *contextstack), IsFalse());
        });
        it("runs if spec's name matches 'only'", [&](){
          bandit_run_policy policy(skip_pattern.c_str(), only_pattern.c_str());
          AssertThat(policy.should_run("it name matching 'only'", *contextstack), IsTrue());
        });
      
      });
      
    });

  
  });

});

