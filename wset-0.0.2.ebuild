# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils git

DESCRIPTION="wset is a tool which allows you to compose wallpapers for X"
HOMEPAGE="http://github.com/niamster/wset"

LICENSE="GPL"
SLOT="0"
KEYWORDS="x86"
IUSE=""

EGIT_REPO_URI="git://github.com/niamster/wset.git"
EGIT_BRANCH="wset-0.0.2"

RDEPEND="media-libs/imlib2
    dev-python/pygtk
    x11-base/xorg-server"
DEPEND="${RDEPEND}"

src_unpack() {
   git_src_unpack
}

src_compile() {
    python3 setup.py build || die "Compilation failed"
}

src_install() {
    python3 setup.py install --prefix /usr/ --root "${D}" || die "Installation failed"
}
