ADD = git add .
COMMIT = git commit -m "codexion-update"
PUSH = git push

all:
	@$(ADD)
	@$(COMMIT)
	@$(PUSH)